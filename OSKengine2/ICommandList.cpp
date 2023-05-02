#include "ICommandList.h"

#include "IGpuImage.h"
#include "OSKengine.h"
#include "IRenderer.h"

#include "Material.h"
#include "MaterialLayout.h"
#include "MaterialInstance.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ICommandList::~ICommandList() {
	DeleteAllStagingBuffers();
}

void ICommandList::SetGpuImageBarrier(GpuImage* image, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageRange& range) {
	const GpuImageLayout previousLayout = image->_GetLayout(range.baseLayer, range.baseMipLevel);
	SetGpuImageBarrier(image, previousLayout, nextLayout, previous, next, range);
}

void ICommandList::SetGpuImageBarrier(GpuImage* image, GpuImageLayout nextLayout, GpuBarrierInfo next, const GpuImageRange& range) {
	const GpuImageLayout previousLayout = image->_GetLayout(range.baseLayer, range.baseMipLevel);
	SetGpuImageBarrier(image, previousLayout, nextLayout, image->GetCurrentBarrier(), next, range);
}

void ICommandList::RegisterStagingBuffer(OwnedPtr<GpuBuffer> stagingBuffer) {
	stagingBuffersToDelete.Insert(stagingBuffer.GetPointer());
}

void ICommandList::BeginGraphicsRenderpass(RenderTarget* renderpass, const Color& color, bool autoSync) {
	currentRenderpassType = renderpass->GetRenderTargetType();

	const bool isFinal = currentRenderpassType == RenderpassType::FINAL;
	const TSize resourceIndex = isFinal 
		? Engine::GetRenderer()->GetCurrentFrameIndex()
		: Engine::GetRenderer()->GetCurrentResourceIndex();

	DynamicArray<RenderPassImageInfo> colorImages;

	if (isFinal) {
		colorImages.Insert({ Engine::GetRenderer()->_GetSwapchain()->GetImage(resourceIndex), 0 });
	}
	else {
		for (TIndex i = 0; i < renderpass->GetNumColorTargets(); i++)
			colorImages.Insert({ renderpass->GetColorImage(i, resourceIndex), 0});
	}

	BeginGraphicsRenderpass(colorImages, { renderpass->GetDepthImage(resourceIndex), 0 }, color, autoSync);
}

void ICommandList::BindVertexBuffer(const GpuBuffer& buffer) {
	OSK_ASSERT(buffer.HasVertexView(), "El buffer no tiene vertex view.");
	BindVertexBufferRange(buffer, buffer.GetVertexView());
}

void ICommandList::BindIndexBuffer(const GpuBuffer& buffer) {
	OSK_ASSERT(buffer.HasIndexView(), "El buffer no tiene index view.");
	BindIndexBufferRange(buffer, buffer.GetIndexView());
}

void ICommandList::BindMaterial(const Material& material) {
	currentMaterial = &material;

	switch (material.GetMaterialType()) {
		case MaterialType::GRAPHICS: {
			PipelineKey pipelineKey{};
			pipelineKey.depthFormat = currentDepthImage.targetImage->GetFormat();

			for (const auto& colorImg : currentColorImages)
				pipelineKey.colorFormats.Insert(colorImg.targetImage->GetFormat());

			currentPipeline.graphics = material.GetGraphicsPipeline(pipelineKey);

			BindGraphicsPipeline(*material.GetGraphicsPipeline(pipelineKey));
			break;
		}

		case MaterialType::RAYTRACING: 
			BindRayTracingPipeline(*material.GetRaytracingPipeline());
			currentPipeline.raytracing = material.GetRaytracingPipeline();
			break;

		case MaterialType::COMPUTE: BindComputePipeline(*material.GetComputePipeline());
			currentPipeline.compute = material.GetComputePipeline();
			break;
	}
}

void ICommandList::BindMaterialInstance(const MaterialInstance& instance) {
	for (const auto& [name, _] : instance.GetLayout()->GetAllSlots())
		BindMaterialSlot(*instance.GetSlot(name));
}

void ICommandList::PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size) {
	PushMaterialConstants(pushConstName, data, size, 0);
}

void ICommandList::DeleteAllStagingBuffers() {
	stagingBuffersToDelete.Free();
}

void ICommandList::_SetSingleTimeUse() {
	isSingleUse = true;
}

TSize ICommandList::_GetCommandListIndex() const {
	return isSingleUse ? 0 : Engine::GetRenderer()->GetCurrentCommandListIndex();
}


CopyImageInfo CopyImageInfo::CreateDefault1D(uint32_t size) {
	CopyImageInfo output{};
	output.SetCopySize(size);

	return output;
}

CopyImageInfo CopyImageInfo::CreateDefault2D(Vector2ui size) {
	CopyImageInfo output{};
	output.SetCopySize(size);

	return output;
}

CopyImageInfo CopyImageInfo::CreateDefault3D(Vector3ui size) {
	CopyImageInfo output{};
	output.SetCopySize(size);

	return output;
}

void CopyImageInfo::SetCopySize(uint32_t size) {
	copySize = { size, 1u, 1u };
}

void CopyImageInfo::SetCopySize(Vector2ui size) {
	copySize = { size.X, size.Y, 1u };
}

void CopyImageInfo::SetCopySize(Vector3ui size) {
	copySize = size;
}

void CopyImageInfo::SetSourceOffset(uint32_t offset) {
	sourceOffset = { offset, 0, 0 };
}

void CopyImageInfo::SetSourceOffset(Vector2ui offset) {
	sourceOffset = { offset.X, offset.Y, 0 };
}

void CopyImageInfo::SetSourceOffset(Vector3ui offset) {
	sourceOffset = offset;
}

void CopyImageInfo::SetDestinationOffset(uint32_t offset) {
	destinationOffset = { offset, 0, 0 };
}

void CopyImageInfo::SetDestinationOffset(Vector2ui offset) {
	destinationOffset = { offset.X, offset.Y, 0 };
}

void CopyImageInfo::SetDestinationOffset(Vector3ui offset) {
	destinationOffset = offset;
}

void CopyImageInfo::SetCopyAllLevels() {
	numArrayLevels = ALL_ARRAY_LEVELS;
}
