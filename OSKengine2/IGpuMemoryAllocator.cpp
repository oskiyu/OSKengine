#include "IGpuMemoryAllocator.h"

#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"
#include "IGpuDataBuffer.h"
#include "IGpuImage.h"
#include "GpuImageUsage.h"
#include "GpuImageDimensions.h"
#include "Format.h"

#include "IGpuVertexBuffer.h"
#include "IGpuIndexBuffer.h"
#include "IGpuStorageBuffer.h"
#include "IGpuUniformBuffer.h"

#include "ITopLevelAccelerationStructure.h"
#include "IBottomLevelAccelerationStructure.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

TSize IGpuMemoryAllocator::SizeOfMemoryBlockInMb = 16;

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

OwnedPtr<IGpuVertexBuffer> IGpuMemoryAllocator::CreateVertexBuffer(const void* data, TSize vertexSize, TSize numVertices, const VertexInfo& vertexInfo, GpuBufferUsage usage) {
	const TSize bufferSize = numVertices * vertexSize;
	IGpuMemoryBlock* block = GetNextBufferMemoryBlock(
		bufferSize, 
		usage | GpuBufferUsage::VERTEX_BUFFER | GpuBufferUsage::TRANSFER_DESTINATION,
		GpuSharedMemoryType::GPU_ONLY);

	// Buffer temporal sobre el que escribimos los vértices.
	GpuDataBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(data, bufferSize);
	stagingBuffer->Unmap();

	OwnedPtr<IGpuVertexBuffer> output = _CreateVertexBuffer(
		block->GetNextMemorySubblock(bufferSize, 0),
		bufferSize, 
		0, 
		numVertices, 
		vertexInfo);


	// Debemos copiar el buffer temporal al buffer final.

	OwnedPtr<ICommandList> singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList();
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();

	singleTimeCommandList->CopyBufferToBuffer(stagingBuffer, output.GetPointer(), bufferSize, 0, 0);

	singleTimeCommandList->Close();

	singleTimeCommandList->RegisterStagingBuffer(stagingBuffer);

	Engine::GetRenderer()->SubmitSingleUseCommandList(singleTimeCommandList);

	return output;
}

OwnedPtr<IGpuIndexBuffer> IGpuMemoryAllocator::CreateIndexBuffer(const DynamicArray<TIndexSize>& indices, GpuBufferUsage usage) {
	const TSize bufferSize = sizeof(TIndexSize) * indices.GetSize();
	IGpuMemoryBlock* block = GetNextBufferMemoryBlock(
		bufferSize, 
		usage | GpuBufferUsage::INDEX_BUFFER | GpuBufferUsage::TRANSFER_DESTINATION,
		GpuSharedMemoryType::GPU_ONLY);

	GpuDataBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(indices.GetData(), bufferSize);
	stagingBuffer->Unmap();

	OwnedPtr<IGpuIndexBuffer> output = _CreateIndexBuffer(block->GetNextMemorySubblock(bufferSize, 0), bufferSize, 0, indices.GetSize());

	auto singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList();
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();

	singleTimeCommandList->CopyBufferToBuffer(stagingBuffer, output.GetPointer(), bufferSize, 0, 0);

	singleTimeCommandList->Close();

	singleTimeCommandList->RegisterStagingBuffer(stagingBuffer);

	Engine::GetRenderer()->SubmitSingleUseCommandList(singleTimeCommandList);

	return output;
}

void IGpuMemoryAllocator::RemoveMemoryBlock(IGpuMemoryBlock* iblock) {
	std::optional<TIndex> index;
	for (TIndex i = 0; i < imageMemoryBlocks.GetSize(); i++) {
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
		for (TIndex i = 0; i < list.GetSize(); i++) {
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
	for (TIndex i = 0; i < bufferMemoryBlocksInfo.GetSize(); i++) {
		const auto& info = bufferMemoryBlocksInfo[i];

		if (info.sharedType == GpuSharedMemoryType::GPU_AND_CPU
			&& EFTraits::HasFlag(info.usage, GpuBufferUsage::UPLOAD_ONLY)) {
			
			DynamicArray<TIndex> freeableIndices{};

			for (TIndex b = 0; b < bufferMemoryBlocks[i].GetSize(); b++) {
				const auto block = bufferMemoryBlocks[i][b].GetPointer();

				if (block->IsUnused())
					freeableIndices.Insert(b);
			}

			if (freeableIndices.GetSize() > 0)
				for (TIndex b = freeableIndices.GetSize() - 1; b > 0; b--)
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

OwnedPtr<IGpuUniformBuffer> IGpuMemoryAllocator::CreateUniformBuffer(TSize size, GpuBufferUsage usage) {
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::UNIFORM_BUFFER;

	IGpuMemoryBlock* block = GetNextBufferMemoryBlock(size, finalUsage, GpuSharedMemoryType::GPU_AND_CPU);
	const TSize alignment = GetAlignment(0, finalUsage);

	return _CreateUniformBuffer(
		block->GetNextMemorySubblock(size, alignment), size, alignment);
}

OwnedPtr<IGpuStorageBuffer> IGpuMemoryAllocator::CreateStorageBuffer(TSize size, GpuBufferUsage usage) {
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::STORAGE_BUFFER;
	const TSize alignment = GetAlignment(0, finalUsage);

	IGpuMemoryBlock* block = GetNextBufferMemoryBlock(size, finalUsage, GpuSharedMemoryType::GPU_AND_CPU);

	return _CreateStorageBuffer(
		block->GetNextMemorySubblock(size, alignment), size, alignment);
}

OwnedPtr<GpuDataBuffer> IGpuMemoryAllocator::CreateStagingBuffer(TSize size, GpuBufferUsage usage) {
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::TRANSFER_SOURCE | GpuBufferUsage::UPLOAD_ONLY;
	const TSize alignment = GetAlignment(0, finalUsage);

	IGpuMemoryBlock* block = GetNextBufferMemoryBlock(
		size,
		finalUsage,
		GpuSharedMemoryType::GPU_AND_CPU);

	return new GpuDataBuffer(block->GetNextMemorySubblock(size, 0), size, 0);
}

OwnedPtr<GpuDataBuffer> IGpuMemoryAllocator::CreateBuffer(TSize size, TSize alignment, GpuBufferUsage usage, GpuSharedMemoryType memoryType) {
	const TSize finalAlignment = GetAlignment(alignment, usage);
	return new GpuDataBuffer(GetNextBufferMemoryBlock(size, usage, memoryType)->GetNextMemorySubblock(size, finalAlignment), size, finalAlignment);
}

OwnedPtr<ITopLevelAccelerationStructure> IGpuMemoryAllocator::CreateTopAccelerationStructure(DynamicArray<IBottomLevelAccelerationStructure*> bottomStructures) {
	OwnedPtr<ITopLevelAccelerationStructure> output = _CreateTopAccelerationStructure();

	for (auto blas : bottomStructures)
		output->AddBottomLevelAccelerationStructure(blas);

	output->Setup();

	return output;
}

OwnedPtr<IBottomLevelAccelerationStructure> IGpuMemoryAllocator::CreateBottomAccelerationStructure(const IGpuVertexBuffer& vertexBuffer, const IGpuIndexBuffer& indexBuffer) {
	OwnedPtr<IBottomLevelAccelerationStructure> output = _CreateBottomAccelerationStructure();
	output->Setup(vertexBuffer, indexBuffer, RtAccelerationStructureFlags::FAST_TRACE);
	output->SetMatrix(glm::mat4(1.0f));

	return output;
}

IGpuMemoryBlock* IGpuMemoryAllocator::GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	// Miramos todas las listas de bloques para comprobar si hay alguna que
	// tiene bloques con las características necesarias.
	for (TIndex i = 0; i < bufferMemoryBlocksInfo.GetSize(); i++) {
		const auto& blockInfo = bufferMemoryBlocksInfo[i];

		// TODO: permitir reutilización.
		if (blockInfo.usage == usage && blockInfo.sharedType == sharedType) {
			for (const auto& block : bufferMemoryBlocks[i]) {
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

IGpuMemorySubblock* IGpuMemoryAllocator::GetNextBufferMemorySubblock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return GetNextBufferMemoryBlock(size, usage, sharedType)->GetNextMemorySubblock(size, 0);
}

TSize IGpuMemoryAllocator::GetAlignment(TSize originalAlignment, GpuBufferUsage usage) const {
	TSize output = originalAlignment;

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
	output.resolution = Vector3ui{ resolution.X, resolution.Y, 1 };
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

void GpuImageCreateInfo::SetMsaaSamples(TSize msaaSamples) {
	this->msaaSamples = msaaSamples;
}

void GpuImageCreateInfo::SetSamplerDescription(const GpuImageSamplerDesc& description) {
	samplerDesc = description;
}

void GpuImageCreateInfo::SetMemoryType(GpuSharedMemoryType memoryType) {
	this->memoryType = memoryType;
}
