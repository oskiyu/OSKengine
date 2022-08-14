#include "ICommandList.h"

#include "IGpuImage.h"
#include "OSKengine.h"
#include "IRenderer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ICommandList::~ICommandList() {
	DeleteAllStagingBuffers();
}

void ICommandList::SetGpuImageBarrier(GpuImage* image, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageBarrierInfo& prevImageInfo) {
	SetGpuImageBarrier(image, image->GetLayout(), nextLayout, previous, next, prevImageInfo);
}

void ICommandList::RegisterStagingBuffer(OwnedPtr<GpuDataBuffer> stagingBuffer) {
	stagingBuffersToDelete.Insert(stagingBuffer);
}

void ICommandList::BeginGraphicsRenderpass(RenderTarget* renderpass, const Color& color) {
	currentRenderpassType = renderpass->GetRenderTargetType();

	const TSize frameIndex = Engine::GetRenderer()->GetCurrentFrameIndex();
	const bool isFinal = currentRenderpassType == RenderpassType::FINAL;

	DynamicArray<RenderPassImageInfo> colorImages;

	if (isFinal) {
		colorImages.Insert({ Engine::GetRenderer()->_GetSwapchain()->GetImage(frameIndex), 0 });
	}
	else {
		const DynamicArray<GpuImage*> targetImages = renderpass->GetTargetImages(frameIndex);
		for (const auto renderTargetImage : targetImages) {
			colorImages.Insert({ renderTargetImage, 0 });
		}
	}

	BeginGraphicsRenderpass(colorImages, { renderpass->GetDepthImage(frameIndex), 0 }, color);
}

void ICommandList::PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size) {
	PushMaterialConstants(pushConstName, data, size, 0);
}

void ICommandList::DeleteAllStagingBuffers() {
	for (TSize i = 0; i < stagingBuffersToDelete.GetSize(); i++)
		stagingBuffersToDelete.At(i).Delete();

	stagingBuffersToDelete.Free();
}

void ICommandList::_SetSingleTimeUse() {
	isSingleUse = true;
}

TSize ICommandList::GetCommandListIndex() const {
	return isSingleUse ? 0 : Engine::GetRenderer()->GetCurrentCommandListIndex();
}
