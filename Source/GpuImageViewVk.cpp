#include "GpuImageViewVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVk.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

template <OSK::GRAPHICS::VulkanTarget Target>
GpuImageViewVk<Target>::GpuImageViewVk(
	VkImageView view,
	const GpuImage& image,
	const GpuImageViewConfig& config)
	: IGpuImageView(image, config), view(view) {

}

template <OSK::GRAPHICS::VulkanTarget Target>
GpuImageViewVk<Target>::~GpuImageViewVk() {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice();

	if (view != VK_NULL_HANDLE) {
		vkDestroyImageView(logicalDevice, view, nullptr);
		view = VK_NULL_HANDLE;
	}
}

template <OSK::GRAPHICS::VulkanTarget Target>
VkImageView GpuImageViewVk<Target>::GetVkView() const {
	return view;
}

#endif
