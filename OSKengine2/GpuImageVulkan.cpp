#include "GpuImageVulkan.h"

#include <vulkan/vulkan.h>
#include "OSKengine.h"
#include "RendererVulkan.h"
#include "GpuVulkan.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageVulkan::GpuImageVulkan(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format)
	: GpuImage(size, dimension, usage, numLayers, format) {

}

GpuImageVulkan::~GpuImageVulkan() {
	if (image != VK_NULL_HANDLE) {
		vkDestroyImage(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(), image, nullptr);
		image = VK_NULL_HANDLE;
	}
	if (view != VK_NULL_HANDLE) {
		vkDestroyImageView(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(), view, nullptr);
		view = VK_NULL_HANDLE;
	}
	if (sampler != VK_NULL_HANDLE) {
		vkDestroySampler(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(), sampler, nullptr);
		sampler = VK_NULL_HANDLE;
	}
}

void GpuImageVulkan::SetImage(VkImage image) {
	this->image = image;
}

VkImage GpuImageVulkan::GetImage() const {
	return image;
}

void GpuImageVulkan::SetView(VkImageView view) {
	this->view = view;
}

VkImageView GpuImageVulkan::GetView() const {
	return view;
}

void GpuImageVulkan::SetSampler(VkSampler sampler) {
	this->sampler = sampler;
}

VkSampler GpuImageVulkan::GetSampler() const {
	return sampler;
}
