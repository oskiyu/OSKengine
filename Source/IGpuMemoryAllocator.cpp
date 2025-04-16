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

#include "GpuImageLayout.h"

#include "OSKengine.h"
#include "Logger.h"
#include <numeric>

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
	Engine::GetLogger()->InfoLog("\t~IGpuMemoryAllocator");
}

GpuMemoryUsageInfo IGpuMemoryAllocator::GetMemoryUsageInfo() const {
	return device->GetMemoryUsageInfo();
}

UniquePtr<GpuBuffer> IGpuMemoryAllocator::CreateVertexBuffer(const void* data, USize32 vertexSize, USize32 numVertices, const VertexInfo& vertexInfo, GpuQueueType queueType, GpuBufferUsage usage) {
	return CreateVertexBuffer(
		data,
		vertexSize,
		numVertices,
		vertexInfo,
		queueType,
		queueType,
		usage);
}

UniquePtr<GpuBuffer> IGpuMemoryAllocator::CreateVertexBuffer(const void* data, USize32 vertexSize, USize32 numVertices, const VertexInfo& vertexInfo, GpuQueueType transferQueue, GpuQueueType queueType ,  GpuBufferUsage usage) {
	const USize64 bufferSize = numVertices * vertexSize;
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::VERTEX_BUFFER | GpuBufferUsage::TRANSFER_DESTINATION;

	// Buffer temporal sobre el que escribimos los vértices.
	auto stagingBuffer = CreateStagingBuffer(bufferSize, transferQueue);
	stagingBuffer->MapMemory();
	stagingBuffer->Write(data, bufferSize);
	stagingBuffer->Unmap();

	UniquePtr<GpuBuffer> output = CreateBuffer(bufferSize, GPU_MEMORY_NO_ALIGNMENT, finalUsage, GpuSharedMemoryType::GPU_ONLY, transferQueue);

	// Establecemos el vertex view.
	VertexBufferView vertexView{};
	vertexView.numVertices = numVertices;
	vertexView.offsetInBytes = 0;
	vertexView.vertexInfo = vertexInfo;

	output->SetVertexView(vertexView);

	// Debemos copiar el buffer temporal al buffer final.
	UniquePtr<ICommandList> singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList(transferQueue);
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();

	singleTimeCommandList->CopyBufferToBuffer(*stagingBuffer, output.GetPointer(), bufferSize, 0, 0);

	if (transferQueue != queueType) {
		singleTimeCommandList->TransferToQueue(
			output.GetPointer(),
			*Engine::GetRenderer()->GetOptimalQueue(queueType));
	}

	singleTimeCommandList->Close();

	singleTimeCommandList->RegisterStagingBuffer(std::move(stagingBuffer));

	Engine::GetRenderer()->SubmitSingleUseCommandList(std::move(singleTimeCommandList));

	return output;
}

UniquePtr<GpuBuffer> IGpuMemoryAllocator::CreateIndexBuffer(const DynamicArray<TIndexSize>& indices, GpuQueueType queueType, GpuBufferUsage usage) {
	return CreateIndexBuffer(
		indices,
		queueType,
		queueType,
		usage);
}

UniquePtr<GpuBuffer> IGpuMemoryAllocator::CreateIndexBuffer(const DynamicArray<TIndexSize>& indices, GpuQueueType transferQueue, GpuQueueType queueType, GpuBufferUsage usage) {
	const USize64 bufferSize = sizeof(TIndexSize) * indices.GetSize();
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::INDEX_BUFFER | GpuBufferUsage::TRANSFER_DESTINATION;

	auto stagingBuffer = CreateStagingBuffer(bufferSize, transferQueue);
	stagingBuffer->MapMemory();
	stagingBuffer->Write(indices.GetData(), bufferSize);
	stagingBuffer->Unmap();

	UniquePtr<GpuBuffer> output = CreateBuffer(bufferSize, GPU_MEMORY_NO_ALIGNMENT, finalUsage, GpuSharedMemoryType::GPU_ONLY, transferQueue);

	// Establecemos el index view.
	IndexBufferView view{};
	view.numIndices = static_cast<USize32>(indices.GetSize());
	view.offsetInBytes = 0;
	view.type = IndexType::U32;

	output->SetIndexView(view);

	auto singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList(transferQueue);
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();

	singleTimeCommandList->CopyBufferToBuffer(*stagingBuffer, output.GetPointer(), bufferSize, 0, 0);

	singleTimeCommandList->TransferToQueue(output.GetPointer(), *Engine::GetRenderer()->GetOptimalQueue(queueType));

	singleTimeCommandList->Close();

	singleTimeCommandList->RegisterStagingBuffer(std::move(stagingBuffer));

	Engine::GetRenderer()->SubmitSingleUseCommandList(std::move(singleTimeCommandList));

	return output;
}

void IGpuMemoryAllocator::RemoveMemoryBlock(IGpuMemoryBlock* iblock) {
	std::optional<UIndex64> index;
	for (UIndex64 i = 0; i < imageMemoryBlocks.GetSize(); i++) {
		if (imageMemoryBlocks[i] == iblock) {
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

const GpuImage* IGpuMemoryAllocator::GetDefaultNormalTexture() const {
	return m_defaultNormalTexture.GetPointer();
}

void IGpuMemoryAllocator::LoadDefaultNormalTexture() {
	IRenderer* renderer = Engine::GetRenderer();
	UniquePtr<ICommandList> uploadCmdList = renderer->CreateSingleUseCommandList(GpuQueueType::ASYNC_TRANSFER);
	uploadCmdList->Start();

	TByte data[4] = {
		127, // R: x
		127, // G: y
		255, // B: z
		255	 // A
	};

	UniquePtr<GpuBuffer> stagingBuffer = CreateStagingBuffer(sizeof(data), uploadCmdList->GetCompatibleQueueType());
	stagingBuffer->MapMemory();
	stagingBuffer->Write(data, sizeof(data));
	stagingBuffer->Unmap();

	GpuImageCreateInfo imageInfo = GpuImageCreateInfo::CreateDefault2D(
		Vector2ui(1, 1),
		Format::RGBA8_UNORM,
		GpuImageUsage::SAMPLED | GpuImageUsage::TRANSFER_DESTINATION);
	imageInfo.samplerDesc.mipMapMode = GpuImageMipmapMode::NONE;
	imageInfo.samplerDesc.addressMode = GpuImageAddressMode::REPEAT;
	m_defaultNormalTexture = CreateImage(imageInfo);

	uploadCmdList->SetGpuImageBarrier(
		m_defaultNormalTexture.GetPointer(),
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	uploadCmdList->CopyBufferToImage(stagingBuffer.GetValue(), m_defaultNormalTexture.GetPointer());

	uploadCmdList->SetGpuImageBarrier(
		m_defaultNormalTexture.GetPointer(),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE));

	uploadCmdList->Close();

	renderer->SubmitSingleUseCommandList(std::move(uploadCmdList));
}

UniquePtr<GpuImage> IGpuMemoryAllocator::CreateCubemapImage(const Vector2ui& faceSize, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, GpuQueueType queueType, GpuImageSamplerDesc samplerDesc) {
	if (!EFTraits::HasFlag(usage, GpuImageUsage::CUBEMAP))
		EFTraits::AddFlag(&usage, GpuImageUsage::CUBEMAP);

	GpuImageCreateInfo info = GpuImageCreateInfo::CreateDefault2D(faceSize, format, usage);
	info.memoryType = sharedType;
	info.samplerDesc = samplerDesc;
	info.numLayers = 6;
	info.queueType = queueType;

	return CreateImage(info);
}

UniquePtr<GpuBuffer> IGpuMemoryAllocator::CreateUniformBuffer(USize64 size, GpuQueueType queueType, GpuBufferUsage usage) {
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::UNIFORM_BUFFER;

	return CreateBuffer(size, GPU_MEMORY_NO_ALIGNMENT, finalUsage, GpuSharedMemoryType::GPU_AND_CPU, queueType);
}

UniquePtr<GpuBuffer> IGpuMemoryAllocator::CreateStorageBuffer(USize64 size, GpuQueueType queueType, GpuBufferUsage usage) {
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::STORAGE_BUFFER;

	return CreateBuffer(size, GPU_MEMORY_NO_ALIGNMENT, finalUsage, GpuSharedMemoryType::GPU_AND_CPU, queueType);
}

UniquePtr<GpuBuffer> IGpuMemoryAllocator::CreateStagingBuffer(USize64 size, GpuQueueType queueType, GpuBufferUsage usage) {
	const GpuBufferUsage finalUsage = usage | GpuBufferUsage::TRANSFER_SOURCE | GpuBufferUsage::UPLOAD_ONLY;
	
	return CreateBuffer(size, GPU_MEMORY_NO_ALIGNMENT, finalUsage, GpuSharedMemoryType::GPU_AND_CPU, queueType);
}

UniquePtr<GpuBuffer> IGpuMemoryAllocator::CreateBuffer(USize64 size, USize64 alignment, GpuBufferUsage usage, GpuSharedMemoryType memoryType, GpuQueueType queueType) {
	const USize64 finalAlignment = GetAlignment(alignment, usage);
	IGpuMemoryBlock* block = GetNextBufferMemoryBlock(size, usage, memoryType);
	UniquePtr<IGpuMemorySubblock> subblock = block->GetNextMemorySubblock(size, finalAlignment);

	return MakeUnique<GpuBuffer>(std::move(subblock), size, finalAlignment, Engine::GetRenderer()->GetOptimalQueue(queueType));
}

UniquePtr<ITopLevelAccelerationStructure> IGpuMemoryAllocator::CreateTopAccelerationStructure(DynamicArray<IBottomLevelAccelerationStructure*> bottomStructures) {
	UniquePtr<ITopLevelAccelerationStructure> output = _CreateTopAccelerationStructure();

	for (auto blas : bottomStructures)
		output->AddBottomLevelAccelerationStructure(blas);

	output->Setup();

	return output;
}

UniquePtr<IBottomLevelAccelerationStructure> IGpuMemoryAllocator::CreateBottomAccelerationStructure(
	const GpuBuffer& vertexBuffer,
	const VertexBufferView& vertexView,
	const GpuBuffer& indexBuffer,
	const IndexBufferView& indexView) 
{
	UniquePtr<IBottomLevelAccelerationStructure> output = _CreateBottomAccelerationStructure();
	output->Setup(vertexBuffer, vertexView, indexBuffer, indexView, RtAccelerationStructureFlags::FAST_TRACE);
	output->SetMatrix(glm::mat4(1.0f));

	return output;
}

UniquePtr<IBottomLevelAccelerationStructure> IGpuMemoryAllocator::CreateBottomAccelerationStructure(const GpuBuffer& vertexBuffer, const GpuBuffer& indexBuffer) {
	return CreateBottomAccelerationStructure(vertexBuffer, vertexBuffer.GetVertexView(), indexBuffer, indexBuffer.GetIndexView());
}

IGpuMemoryBlock* IGpuMemoryAllocator::GetNextBufferMemoryBlock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	std::lock_guard lock(m_memoryAllocationMutex.mutex);

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
	UniquePtr<IGpuMemoryBlock> newBlock = CreateNewBufferBlock(size, usage, sharedType);
	auto* output = newBlock.GetPointer();

	// Engine::GetLogger()->InfoLog("Creado bloque de memoria GPU.");
	// Engine::GetLogger()->InfoLog("	Tipo: " + ToString(sharedType)); @bug ???
	// Engine::GetLogger()->InfoLog("	Uso: " + ToString(usage)); @bug ???
	// Engine::GetLogger()->InfoLog("	Tamaño: " + std::to_string(size));

	bufferMemoryBlocksInfo.Insert(GpuBufferMemoryBlockInfo{ size, usage, sharedType });
	bufferMemoryBlocks.Insert({  });
	bufferMemoryBlocks.Peek().Insert(std::move(newBlock));

	return output;
}

UniquePtr<IGpuMemorySubblock> IGpuMemoryAllocator::GetNextBufferMemorySubblock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return GetNextBufferMemoryBlock(size, usage, sharedType)->GetNextMemorySubblock(size, 0);
}

USize64 IGpuMemoryAllocator::GetAlignment(USize64 originalAlignment, GpuBufferUsage usage) const {
	const USize64 vertexAlignment = EFTraits::HasFlag(usage, GpuBufferUsage::VERTEX_BUFFER)
		? device->GetMemoryAlignments().minVertexBufferAlignment
		: 1;

	const USize64 indexAlignment = EFTraits::HasFlag(usage, GpuBufferUsage::INDEX_BUFFER)
		? device->GetMemoryAlignments().minIndexBufferAlignment
		: 1;

	const USize64 uniformAlignment = EFTraits::HasFlag(usage, GpuBufferUsage::UNIFORM_BUFFER)
		? device->GetMemoryAlignments().minUniformBufferAlignment
		: 1;

	const USize64 storageAlignment = EFTraits::HasFlag(usage, GpuBufferUsage::STORAGE_BUFFER)
		? device->GetMemoryAlignments().minStorageBufferAlignment
		: 1;

	return std::lcm(
		originalAlignment,
		std::lcm(
			std::lcm(vertexAlignment, indexAlignment),
			std::lcm(uniformAlignment, storageAlignment)));
}
