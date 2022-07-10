#include "ISwapchain.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

PresentMode ISwapchain::GetCurrentPresentMode() const {
	return mode;
}

unsigned int ISwapchain::GetImageCount() const {
	return imageCount;
}

unsigned int ISwapchain::GetCurrentFrameIndex() const {
	return currentFrameIndex;
}

GpuImage* ISwapchain::GetImage(unsigned int index) const {
	return images[index].GetPointer();
}

IRenderpass* ISwapchain::GetTargetRenderpass() const {
	return targetRenderpass;
}

void ISwapchain::SetTargetRenderpass(IRenderpass* renderpass) {
	targetRenderpass = renderpass;
}

Format ISwapchain::GetColorFormat() const {
	return colorFormat;
}
