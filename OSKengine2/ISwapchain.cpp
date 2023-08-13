#include "ISwapchain.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuImage.h"
#include "ICommandList.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


void ISwapchain::TakeScreenshot() {
	GpuImage* swapchainImage = images[Engine::GetRenderer()->GetCurrentFrameIndex()].GetPointer();

	GpuImageCreateInfo intermediateImageInfo = GpuImageCreateInfo::CreateDefault2D(
		swapchainImage->GetSize2D(),
		Format::RGBA8_SRGB,
		GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION
	);
	intermediateImageInfo.memoryType = GpuSharedMemoryType::GPU_AND_CPU;
	
	UniquePtr<GpuImage> intermediateImage = Engine::GetRenderer()->GetAllocator()->CreateImage(intermediateImageInfo).GetPointer();

	OwnedPtr<ICommandList> commandList = Engine::GetRenderer()->CreateSingleUseCommandList();
	commandList->Reset();
	commandList->Start();

	commandList->SetGpuImageBarrier(
		swapchainImage,
		GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ)
	);

	// @todo commandList->RawCopyImageToImage();

	commandList->Close();
}

PresentMode ISwapchain::GetCurrentPresentMode() const {
	return mode;
}

unsigned int ISwapchain::GetImageCount() const {
	return imageCount;
}

unsigned int ISwapchain::GetCurrentFrameIndex() const {
	return currentFrameIndex;
}

Format ISwapchain::GetColorFormat() const {
	return colorFormat;
}
