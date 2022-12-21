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
	stagingBuffersToDelete.Insert(stagingBuffer.GetPointer());
}

void ICommandList::BeginGraphicsRenderpass(RenderTarget* renderpass, const Color& color) {
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

	BeginGraphicsRenderpass(colorImages, { renderpass->GetDepthImage(resourceIndex), 0 }, color);
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

TSize ICommandList::GetCommandListIndex() const {
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
