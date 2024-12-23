#include "ICommandList.h"

#include "IGpuImage.h"
#include "OSKengine.h"
#include "IRenderer.h"

#include "Material.h"
#include "MaterialLayout.h"
#include "MaterialInstance.h"

#include "GpuBufferRange.h"

#include "CommandListExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ICommandList::ICommandList(ICommandPool* commandPool) : m_ownerPool(commandPool) {

}

ICommandList::~ICommandList() {
	DeleteAllStagingBuffers();
}


void ICommandList::_SetSingleTime() {
	m_isSingleUse = true;
}

void ICommandList::SetViewport(const Viewport& viewport) {
	m_currentViewport = viewport;
}

const Viewport& ICommandList::GetCurrentViewport() const {
	return m_currentViewport;
}

void ICommandList::SetGpuImageBarrier(GpuImage* image, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageRange& range, const ResourceQueueTransferInfo queueTranfer) {
	const GpuImageLayout previousLayout = image->_GetLayout(range.baseLayer, range.baseMipLevel);
	SetGpuImageBarrier(image, previousLayout, nextLayout, previous, next, range, queueTranfer);
}

void ICommandList::SetGpuImageBarrier(GpuImage* image, GpuImageLayout nextLayout, GpuBarrierInfo next, const GpuImageRange& range, const ResourceQueueTransferInfo queueTranfer) {
	const GpuImageLayout previousLayout = image->_GetLayout(range.baseLayer, range.baseMipLevel);
	SetGpuImageBarrier(image, previousLayout, nextLayout, image->GetCurrentBarrier(), next, range, queueTranfer);
}

void ICommandList::SetGpuBufferBarrier(GpuBuffer* buffer, const GpuBufferRange& range, GpuBarrierInfo next, const ResourceQueueTransferInfo queueTranfer) {
	SetGpuBufferBarrier(
		buffer,
		range,
		buffer->GetCurrentBarrier(),
		next,
		queueTranfer);
}

void ICommandList::RegisterStagingBuffer(OwnedPtr<GpuBuffer> stagingBuffer) {
	m_stagingBuffersToDelete.Insert(stagingBuffer.GetPointer());
}

void ICommandList::BeginGraphicsRenderpass(RenderTarget* renderpass, const Color& color, bool autoSync) {
	m_currentlyBoundRenderTargetType = renderpass->GetRenderTargetType();

	const bool isFinal = m_currentlyBoundRenderTargetType == RenderpassType::FINAL;
	const UIndex32 resourceIndex = isFinal
		? Engine::GetRenderer()->GetCurrentFrameIndex()
		: Engine::GetRenderer()->GetCurrentResourceIndex();

	DynamicArray<RenderPassImageInfo> colorImages;

	if (isFinal) {
		colorImages.Insert({ Engine::GetRenderer()->_GetSwapchain()->GetImage(resourceIndex), 0 });
	}
	else {
		for (UIndex32 i = 0; i < renderpass->GetNumColorTargets(); i++) {
			colorImages.Insert({ renderpass->GetColorImage(i), 0 });
		}
	}

	BeginGraphicsRenderpass(colorImages, { renderpass->GetDepthImage(), 0 }, color, autoSync);
}

void ICommandList::BindVertexBuffer(const GpuBuffer& buffer) {
	OSK_ASSERT(buffer.HasVertexView(), InvalidVertexBufferException());
	BindVertexBufferRange(buffer, buffer.GetVertexView());
}

void ICommandList::BindIndexBuffer(const GpuBuffer& buffer) {
	OSK_ASSERT(buffer.HasIndexView(), InvalidIndexBufferException());
	BindIndexBufferRange(buffer, buffer.GetIndexView());
}

void ICommandList::BindMaterial(const Material& material) {
	m_currentlyBoundMaterial = &material;

	switch (material.GetMaterialType()) {
		case MaterialType::GRAPHICS: {
			PipelineKey pipelineKey{};
			pipelineKey.depthFormat = m_currentlyBoundDepthImage.targetImage->GetFormat();

			for (const auto& colorImg : m_currentlyBoundColorImages)
				pipelineKey.colorFormats.Insert(colorImg.targetImage->GetFormat());

			m_currentPipeline.graphics = material.GetGraphicsPipeline(pipelineKey);

			BindGraphicsPipeline(*material.GetGraphicsPipeline(pipelineKey));
			break;
		}

		case MaterialType::MESH: {
			PipelineKey pipelineKey{};
			pipelineKey.depthFormat = m_currentlyBoundDepthImage.targetImage->GetFormat();

			for (const auto& colorImg : m_currentlyBoundColorImages)
				pipelineKey.colorFormats.Insert(colorImg.targetImage->GetFormat());

			m_currentPipeline.mesh = material.GetMeshPipeline(pipelineKey);

			BindMeshPipeline(*material.GetMeshPipeline(pipelineKey));
			break;
		}

		case MaterialType::RAYTRACING: 
			BindRayTracingPipeline(*material.GetRaytracingPipeline());
			m_currentPipeline.raytracing = material.GetRaytracingPipeline();
			break;

		case MaterialType::COMPUTE: 
			BindComputePipeline(*material.GetComputePipeline());
			m_currentPipeline.compute = material.GetComputePipeline();
			break;
	}
}

void ICommandList::BindMaterialInstance(const MaterialInstance& instance) {
	for (const auto& [name, _] : instance.GetLayout()->GetAllSlots())
		BindMaterialSlot(*instance.GetSlot(name));
}

void ICommandList::PushMaterialConstants(const std::string& pushConstName, const void* data, USize32 size) {
	PushMaterialConstants(pushConstName, data, size, 0);
}

void ICommandList::DeleteAllStagingBuffers() {
	m_stagingBuffersToDelete.Free();
}

void ICommandList::_SetSingleTimeUse() {
	m_isSingleUse = true;
}

ICommandPool* ICommandList::GetOwnerPool() {
	return m_ownerPool;
}

const ICommandPool* ICommandList::GetOwnerPool() const {
	return m_ownerPool;
}

GpuQueueType ICommandList::GetCompatibleQueueType() const {
	return m_ownerPool->GetLinkedQueueType();
}

UIndex32 ICommandList::_GetCommandListIndex() const {
	return m_isSingleUse ? 0 : Engine::GetRenderer()->GetCurrentCommandListIndex();
}

void ICommandList::TransferToQueue(GpuImage* image, const ICommandQueue& sourceQueue, const ICommandQueue& targetQueue) {
	if (&sourceQueue == &targetQueue) {
		return;
	}

	SetGpuImageBarrier(
		image,
		image->_GetLayout(0, 0),
		GpuBarrierInfo(GpuCommandStage::ALL, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::ALL, GpuAccessStage::NONE),
		GpuImageRange{},
		ResourceQueueTransferInfo::FromTo(&sourceQueue, &targetQueue));

	image->_UpdateOwnerQueue(std::addressof(targetQueue));
}

void ICommandList::TransferToQueue(GpuImage* image, const ICommandQueue& targetQueue) {
	TransferToQueue(image, *image->GetOwnerQueue(), targetQueue);
}

void ICommandList::TransferToQueue(GpuBuffer* buffer, const ICommandQueue& sourceQueue, const ICommandQueue& targetQueue) {
	SetGpuBufferBarrier(
		buffer,
		buffer->GetWholeBufferRange(),
		GpuBarrierInfo(GpuCommandStage::ALL, GpuAccessStage::NONE),
		ResourceQueueTransferInfo::FromTo(
			&sourceQueue,
			&targetQueue));
}

void ICommandList::TransferToQueue(GpuBuffer* buffer, const ICommandQueue& targetQueue) {
	TransferToQueue(
		buffer,
		*buffer->GetOwnerQueue(),
		targetQueue);
}