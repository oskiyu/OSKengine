#include "GpuImageViewVulkan.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageViewVulkan::GpuImageViewVulkan(VkImageView view, SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage)
	: IGpuImageView(channel, arrayType, baseArrayLevel, layerCount, usage), view(view) {

}

GpuImageViewVulkan::~GpuImageViewVulkan() {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();

	if (view != VK_NULL_HANDLE)
		vkDestroyImageView(logicalDevice, view, nullptr);
}

VkImageView GpuImageViewVulkan::GetVkView() const {
	return view;
}
