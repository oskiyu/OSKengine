#include "IGpuMemoryAllocator.h"

#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"
#include "GpuBuffer.h"
#include "IGpuImage.h"
#include "GpuImageUsage.h"
#include "GpuImageDimensions.h"
#include "Format.h"

#include "ITopLevelAccelerationStructure.h"
#include "IBottomLevelAccelerationStructure.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

USize64 const IGpuMemoryAllocator::SizeOfMemoryBlockInMb = 16u;

bool GpuBufferMemoryBlockInfo::operator==(const GpuBufferMemoryBlockInfo& other) const {
	return size == other.size && usage == other.usage && sharedType == other.sharedType;
}

IGpuMemoryAllocator::IGpuMemoryAllocator(IGpu* device) : device(device) {
	bufferMemoryBlocks = {};
}

IGpuMemoryAllocator::~IGpuMemoryAllocator() {
	bufferMemoryBlocks.Free();
	imageMemoryBlocks.Free();
}

GpuMemoryUsageInfo IGpuMemoryAllocator::GetMemoryUsageInfo() const {
	return device->GetMemoryUsageInfo();
}

OwnedPtr<GpuBuffer> IGpuMemoryAllocator::CreateVertexBuffer(const void* data, USize32 vertexSize, USize32 numVertices, const VertexInfo& vertexInfo, GpuBufferUsage usage) {
	const USize64 bufferSize = numVertices * vertexSize;
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::VERTEX_BUFFER | GpuBufferUsage::TRANSFER_DESTINATION;

	// Buffer temporal sobre el que escribimos los vértices.
	GpuBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(data, bufferSize);
	stagingBuffer->Unmap();

	OwnedPtr<GpuBuffer> output = CreateBuffer(bufferSize, 0, finalUsage, GpuSharedMemoryType::GPU_ONLY);

	// Establecemos el vertex view.
	VertexBufferView vertexView{};
	vertexView.numVertices = numVertices;
	vertexView.offsetInBytes = 0;
	vertexView.vertexInfo = vertexInfo;

	output->SetVertexView(vertexView);

	// Debemos copiar el buffer temporal al buffer final.
	OwnedPtr<ICommandList> singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList();
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();

	singleTimeCommandList->CopyBufferToBuffer(*stagingBuffer, output.GetPointer(), bufferSize, 0, 0);

	singleTimeCommandList->Close();

	singleTimeCommandList->RegisterStagingBuffer(stagingBuffer);

	Engine::GetRenderer()->SubmitSingleUseCommandList(singleTimeCommandList);

	return output;
}

OwnedPtr<GpuBuffer> IGpuMemoryAllocator::CreateIndexBuffer(const DynamicArray<TIndexSize>& indices, GpuBufferUsage usage) {
	const USize64 bufferSize = sizeof(TIndexSize) * indices.GetSize();
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::INDEX_BUFFER | GpuBufferUsage::TRANSFER_DESTINATION;

	GpuBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(indices.GetData(), bufferSize);
	stagingBuffer->Unmap();

	OwnedPtr<GpuBuffer> output = CreateBuffer(bufferSize, 0, finalUsage, GpuSharedMemoryType::GPU_ONLY);

	// Establecemos el index view.
	IndexBufferView view{};
	view.numIndices = static_cast<USize32>(indices.GetSize());
	view.offsetInBytes = 0;
	view.type = IndexType::U32;

	output->SetIndexView(view);

	auto singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList();
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();

	singleTimeCommandList->CopyBufferToBuffer(*stagingBuffer, output.GetPointer(), bufferSize, 0, 0);

	singleTimeCommandList->Close();

	singleTimeCommandList->RegisterStagingBuffer(stagingBuffer);

	Engine::GetRenderer()->SubmitSingleUseCommandList(singleTimeCommandList);

	return output;
}

void IGpuMemoryAllocator::RemoveMemoryBlock(IGpuMemoryBlock* iblock) {
	std::optional<UIndex64> index;
	for (UIndex64 i = 0; i < imageMemoryBlocks.GetSize(); i++) {
		if (imageMemoryBlocks[i].GetPointer() == iblock) {
			index = i;
			break;
		}
	}

	if (index.has_value()) {
		imageMemoryBlocks.RemoveAt(index.value());
		return;
	}

	for (auto& list : bufferMemoryBlocks) {
		for (UIndex64 i = 0; i < list.GetSize(); i++) {
			if (list[i].GetPointer() == iblock) {
				index = i;
				break;
			}
		}

		if (index.has_value()) {
			imageMemoryBlocks.RemoveAt(index.value());
			return;
		}
	}
}

void IGpuMemoryAllocator::FreeStagingMemory() {
	for (UIndex64 i = 0; i < bufferMemoryBlocksInfo.GetSize(); i++) {
		const auto& info = bufferMemoryBlocksInfo[i];

		if (info.sharedType == GpuSharedMemoryType::GPU_AND_CPU
			&& EFTraits::HasFlag(info.usage, GpuBufferUsage::UPLOAD_ONLY)) {
			
			DynamicArray<UIndex64> freeableIndices{};

			for (UIndex64 b = 0; b < bufferMemoryBlocks[i].GetSize(); b++) {
				const auto block = bufferMemoryBlocks[i][b].GetPointer();

				if (block->IsUnused())
					freeableIndices.Insert(b);
			}

			if (freeableIndices.GetSize() > 0)
				for (UIndex64 b = freeableIndices.GetSize() - 1; b > 0; b--)
					bufferMemoryBlocks[i].RemoveAt(b);
		}
	}
}

OwnedPtr<GpuImage> IGpuMemoryAllocator::CreateCubemapImage(const Vector2ui& faceSize, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, GpuImageSamplerDesc samplerDesc) {
	if (!EFTraits::HasFlag(usage, GpuImageUsage::CUBEMAP))
		EFTraits::AddFlag(&usage, GpuImageUsage::CUBEMAP);

	GpuImageCreateInfo info = GpuImageCreateInfo::CreateDefault2D(faceSize, format, usage);
	info.memoryType = sharedType;
	info.samplerDesc = samplerDesc;
	info.numLayers = 6;

	return CreateImage(info);
}

OwnedPtr<GpuBuffer> IGpuMemoryAllocator::CreateUniformBuffer(USize64 size, GpuBufferUsage usage) {
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::UNIFORM_BUFFER;

	return CreateBuffer(size, 0, finalUsage, GpuSharedMemoryType::GPU_AND_CPU);
}

OwnedPtr<GpuBuffer> IGpuMemoryAllocator::CreateStorageBuffer(USize64 size, GpuBufferUsage usage) {
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::STORAGE_BUFFER;

	return CreateBuffer(size, 0, finalUsage, GpuSharedMemoryType::GPU_AND_CPU);
}

OwnedPtr<GpuBuffer> IGpuMemoryAllocator::CreateStagingBuffer(USize64 size, GpuBufferUsage usage) {
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::TRANSFER_SOURCE | GpuBufferUsage::UPLOAD_ONLY;
	
	return CreateBuffer(size, 0, finalUsage, GpuSharedMemoryType::GPU_AND_CPU);
}

OwnedPtr<GpuBuffer> IGpuMemoryAllocator::CreateBuffer(USize64 size, USize64 alignment, GpuBufferUsage usage, GpuSharedMemoryType memoryType) {
	const USize64 finalAlignment = GetAlignment(alignment, usage);
	IGpuMemoryBlock* block = GetNextBufferMemoryBlock(size, usage, memoryType);
	OwnedPtr<IGpuMemorySubblock> subblock = block->GetNextMemorySubblock(size, finalAlignment);

	return new GpuBuffer(subblock, size, finalAlignment);
}

OwnedPtr<ITopLevelAccelerationStructure> IGpuMemoryAllocator::CreateTopAccelerationStructure(DynamicArray<IBottomLevelAccelerationStructure*> bottomStructures) {
	OwnedPtr<ITopLevelAccelerationStructure> output = _CreateTopAccelerationStructure();

	for (auto blas : bottomStructures)
		output->AddBottomLevelAccelerationStructure(blas);

	output->Setup();

	return output;
}

OwnedPtr<IBottomLevelAccelerationStructure> IGpuMemoryAllocator::CreateBottomAccelerationStructure(
	const GpuBuffer& vertexBuffer,
	const VertexBufferView& vertexView,
	const GpuBuffer& indexBuffer,
	const IndexBufferView& indexView) 
{
	OwnedPtr<IBottomLevelAccelerationStructure> output = _CreateBottomAccelerationStructure();
	output->Setup(vertexBuffer, vertexView, indexBuffer, indexView, RtAccelerationStructureFlags::FAST_TRACE);
	output->SetMatrix(glm::mat4(1.0f));

	return output;
}

OwnedPtr<IBottomLevelAccelerationStructure> IGpuMemoryAllocator::CreateBottomAccelerationStructure(const GpuBuffer& vertexBuffer, const GpuBuffer& indexBuffer) {
	return CreateBottomAccelerationStructure(vertexBuffer, vertexBuffer.GetVertexView(), indexBuffer, indexBuffer.GetIndexView());
}

IGpuMemoryBlock* IGpuMemoryAllocator::GetNextBufferMemoryBlock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	// Miramos todas las listas de bloques para comprobar si hay alguna que
	// tiene bloques con las características necesarias.
	for (UIndex64 i = 0; i < bufferMemoryBlocksInfo.GetSize(); i++) {
		const auto& blockInfo = bufferMemoryBlocksInfo[i];

		// TODO: permitir reutilización.
		if (blockInfo.usage == usage && blockInfo.sharedType == sharedType) {
			for (auto& block : bufferMemoryBlocks[i]) {
				if (block->GetAvailableSpace() >= size)
					return block.GetPointer();
			}
		}
	}

	// Creamos uno nuevo
	if (size < IGpuMemoryAllocator::SizeOfMemoryBlockInMb * 1024 * 1024)
		size = IGpuMemoryAllocator::SizeOfMemoryBlockInMb * 1024 * 1024;

	// Creamos un nuevo bloque.
	OwnedPtr<IGpuMemoryBlock> newBlock = CreateNewBufferBlock(size, usage, sharedType);

	// Engine::GetLogger()->InfoLog("Creado bloque de memoria GPU.");
	// Engine::GetLogger()->InfoLog("	Tipo: " + ToString(sharedType)); @bug ???
	// Engine::GetLogger()->InfoLog("	Uso: " + ToString(usage)); @bug ???
	// Engine::GetLogger()->InfoLog("	Tamaño: " + std::to_string(size));

	bufferMemoryBlocksInfo.Insert(GpuBufferMemoryBlockInfo{ size, usage, sharedType });
	bufferMemoryBlocks.Insert({  });
	bufferMemoryBlocks.Peek().Insert(newBlock.GetPointer());

	return newBlock.GetPointer();
}

IGpuMemorySubblock* IGpuMemoryAllocator::GetNextBufferMemorySubblock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return GetNextBufferMemoryBlock(size, usage, sharedType)->GetNextMemorySubblock(size, 0);
}

USize64 IGpuMemoryAllocator::GetAlignment(USize64 originalAlignment, GpuBufferUsage usage) const {
	USize64 output = originalAlignment;

	// @todo Combinación correcta de alignments.

	if (EFTraits::HasFlag(usage, GpuBufferUsage::VERTEX_BUFFER))
		output = glm::max(output, minVertexBufferAlignment);

	if (EFTraits::HasFlag(usage, GpuBufferUsage::INDEX_BUFFER))
		output = glm::max(output, minIndexBufferAlignment);

	if (EFTraits::HasFlag(usage, GpuBufferUsage::UNIFORM_BUFFER))
		output = glm::max(output, minUniformBufferAlignment);

	if (EFTraits::HasFlag(usage, GpuBufferUsage::STORAGE_BUFFER))
		output = glm::max(output, minStorageBufferAlignment);

	return output;
}

GpuImageCreateInfo GpuImageCreateInfo::CreateDefault1D(uint32_t resolution, Format format, GpuImageUsage usage) {
	GpuImageCreateInfo output{};
	output.resolution = Vector3ui{ resolution, 1, 1 };
	output.format = format;
	output.usage = usage;
	output.dimension = GpuImageDimension::d1D;

	return output;
}

GpuImageCreateInfo GpuImageCreateInfo::CreateDefault2D(const Vector2ui& resolution, Format format, GpuImageUsage usage) {
	GpuImageCreateInfo output{};
	output.resolution = Vector3ui{ resolution.x, resolution.y, 1 };
	output.format = format;
	output.usage = usage;
	output.dimension = GpuImageDimension::d2D;

	return output;
}

GpuImageCreateInfo GpuImageCreateInfo::CreateDefault3D(const Vector3ui& resolution, Format format, GpuImageUsage usage) {
	GpuImageCreateInfo output{};
	output.resolution = resolution;
	output.format = format;
	output.usage = usage;
	output.dimension = GpuImageDimension::d3D;

	return output;
}

void GpuImageCreateInfo::SetMsaaSamples(USize32 msaaSamples) {
	this->msaaSamples = msaaSamples;
}

void GpuImageCreateInfo::SetSamplerDescription(const GpuImageSamplerDesc& description) {
	samplerDesc = description;
}

void GpuImageCreateInfo::SetMemoryType(GpuSharedMemoryType memoryType) {
	this->memoryType = memoryType;
}
