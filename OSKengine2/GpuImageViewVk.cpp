#include "GpuImageViewVk.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVk.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageViewVk::GpuImageViewVk(
	VkImageView view,
	const GpuImage& image,
	const GpuImageViewConfig& config)
	: IGpuImageView(image, config), view(view) {

}

GpuImageViewVk::~GpuImageViewVk() {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	if (view != VK_NULL_HANDLE) {
		vkDestroyImageView(logicalDevice, view, nullptr);
		view = VK_NULL_HANDLE;
	}
}

VkImageView GpuImageViewVk::GetVkView() const {
	return view;
}
