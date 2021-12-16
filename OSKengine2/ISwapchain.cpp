#include "ISwapchain.h"

using namespace OSK;

unsigned int ISwapchain::GetImageCount() const {
	return imageCount;
}

unsigned int ISwapchain::GetCurrentFrameIndex() const {
	return currentFrameIndex;
}

GpuImage* ISwapchain::GetImage(unsigned int index) const {
	return images[index].GetPointer();
}
