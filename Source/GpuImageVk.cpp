#include "GpuImageVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>

#include "OSKengine.h"
#include "RendererVk.h"
#include "GpuVk.h"
#include "Logger.h"

#include "Assert.h"
#include "UnreachableException.h"

#include "FormatVk.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypeVk.h"
#include "GpuImageDimensions.h"
#include "CommandQueueVk.h"
#include "GpuImageViewVk.h"

#include <format>

#include "RendererExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


template <VulkanTarget Target>
VkImageAspectFlags GpuImageVk<Target>::GetAspectFlags(SampledChannel channel) {
	VkImageAspectFlags aspectMask = 0;

	if (EFTraits::HasFlag(channel, SampledChannel::COLOR))
		aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
	if (EFTraits::HasFlag(channel, SampledChannel::DEPTH))
		aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
	if (EFTraits::HasFlag(channel, SampledChannel::STENCIL))
		aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

	return aspectMask;
}


template <VulkanTarget Target>
GpuImageVk<Target>::GpuImageVk(const GpuImageCreateInfo& info, const ICommandQueue* ownerQueue) : GpuImage(info, ownerQueue) {

}

template <VulkanTarget Target>
GpuImageVk<Target>::~GpuImageVk() {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice();

	if (m_image != VK_NULL_HANDLE)
		vkDestroyImage(logicalDevice, m_image, nullptr);
}


/* VULKAN SPECIFICS */

template <VulkanTarget Target>
void GpuImageVk<Target>::CreateVkImage() {
	VkImageCreateInfo imageInfo{};
	
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = GetVkImageType();

	imageInfo.extent.width = GetSize3D().x;
	imageInfo.extent.height = GetSize3D().y;
	imageInfo.extent.depth = GetSize3D().z;

	imageInfo.mipLevels = GetMipLevels();
	imageInfo.arrayLayers = GetNumLayers();

	imageInfo.format = GetFormatVk(GetFormat());
	imageInfo.tiling = GetImageTiling() == GpuImageTiling::OPTIMAL ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = GetGpuImageUsageVk(GetUsage());
	imageInfo.samples = (VkSampleCountFlagBits)GetMsaaSampleCount();

	std::set<UIndex32> queueIndices;

	const auto* renderer = Engine::GetRenderer();

	if (GetOwnerQueue()->GetQueueType() == GpuQueueType::ASYNC_TRANSFER && renderer->HasTransferOnlyQueue()) {
		queueIndices.insert(renderer->GetTransferOnlyQueue()->As<CommandQueueVk<Target>>()->GetFamily().familyIndex);
	}
	else {
		if (GetOwnerQueue()->GetQueueType() == GpuQueueType::PRESENTATION) {
			if (renderer->UseUnifiedCommandQueue()) {
				queueIndices.insert(renderer->GetUnifiedQueue()->As<CommandQueueVk<Target>>()->GetFamily().familyIndex);
			}
			else {
				queueIndices.insert(renderer->GetGraphicsComputeQueue()->As<CommandQueueVk<Target>>()->GetFamily().familyIndex);
				queueIndices.insert(renderer->GetPresentationQueue()->As<CommandQueueVk<Target>>()->GetFamily().familyIndex);
			}
		}

		if (GetOwnerQueue()->GetQueueType() == GpuQueueType::MAIN) {
			if (renderer->UseUnifiedCommandQueue()) {
				queueIndices.insert(renderer->GetUnifiedQueue()->As<CommandQueueVk<Target>>()->GetFamily().familyIndex);
			}
			else {
				queueIndices.insert(renderer->GetGraphicsComputeQueue()->As<CommandQueueVk<Target>>()->GetFamily().familyIndex);
			}
		}
	}

	DynamicArray<USize32> indices = DynamicArray<USize32>::CreateReserved(static_cast<USize32>(queueIndices.size()));
	for (const auto& index : queueIndices) {
		indices.Insert(index);
	}

	imageInfo.queueFamilyIndexCount = static_cast<USize32>(queueIndices.size());
	imageInfo.pQueueFamilyIndices = indices.GetData();
	imageInfo.sharingMode = queueIndices.size() == 1
		? VK_SHARING_MODE_EXCLUSIVE
		: VK_SHARING_MODE_CONCURRENT;

	// Si la imagen se usará como cubemap, debemos especificarlo.
	imageInfo.flags = EFTraits::HasFlag(GetUsage(), GpuImageUsage::CUBEMAP) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	
	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice();
	const VkResult result = vkCreateImage(device,	&imageInfo, nullptr, &m_image);

	OSK_ASSERT(result == VK_SUCCESS, ImageCreationException(result));
}

template <VulkanTarget Target>
VkImage GpuImageVk<Target>::GetVkImage() const {
	return m_image;
}

template <VulkanTarget Target>
void GpuImageVk<Target>::_SetVkImage(VkImage img) {
	m_image = img;
}

template <VulkanTarget Target>
void GpuImageVk<Target>::SetSwapchainView(VkImageView view) {
	m_swapchainView = view;
}

template <VulkanTarget Target>
VkImageView GpuImageVk<Target>::GetSwapchainView() const {
	return m_swapchainView;
}

template <VulkanTarget Target>
UniquePtr<IGpuImageView> GpuImageVk<Target>::CreateView(const GpuImageViewConfig& viewConfig) const {
	VkImageView view = VK_NULL_HANDLE;
	
	VkImageViewCreateInfo viewInfo{};

	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = m_image;
	viewInfo.viewType = viewConfig.arrayType == SampledArrayType::SINGLE_LAYER ? GetVkImageViewType() : GetVkImageArrayViewType();
	viewInfo.format = GetFormatVk(GetFormat());
	
	VkImageAspectFlags aspectMask = 0;
	if (EFTraits::HasFlag(viewConfig.channel, SampledChannel::COLOR))
		aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
	if (EFTraits::HasFlag(viewConfig.channel, SampledChannel::DEPTH))
		aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
	if (EFTraits::HasFlag(viewConfig.channel, SampledChannel::STENCIL))
		aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

	viewInfo.subresourceRange.aspectMask = aspectMask;

	viewInfo.subresourceRange.baseMipLevel = 
		viewConfig.mipMapMode == GpuImageViewConfig::MipMapMode::ALL_LEVELS
			? 0
			: viewConfig.baseMipLevel;
	viewInfo.subresourceRange.levelCount =
		viewConfig.mipMapMode == GpuImageViewConfig::MipMapMode::ALL_LEVELS
			? GetMipLevels()
			: viewConfig.topMipLevel - viewConfig.baseMipLevel + 1;

	viewInfo.subresourceRange.baseArrayLayer = viewConfig.baseArrayLevel;
	viewInfo.subresourceRange.layerCount = viewConfig.arrayLevelCount;

	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice();
	const VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &view);

	OSK_ASSERT(result == VK_SUCCESS, ImageViewCreationException(result));

	return MakeUnique<GpuImageViewVk<Target>>(view, *this, viewConfig);
}

template <VulkanTarget Target>
void GpuImageVk<Target>::SetDebugName(const std::string& name) {
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = VK_OBJECT_TYPE_IMAGE;
	nameInfo.pNext = nullptr;

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice();

	nameInfo.pObjectName = name.c_str();
	nameInfo.objectHandle = (uint64_t)m_image;

	if (RendererVk<Target>::pvkSetDebugUtilsObjectNameEXT != nullptr)
		RendererVk<Target>::pvkSetDebugUtilsObjectNameEXT(logicalDevice, &nameInfo);

	m_name = name;
}


template <VulkanTarget Target>
VkImageType GpuImageVk<Target>::GetVkImageType() const {
	switch (GetDimension()) {
	case GpuImageDimension::d1D:
		return VK_IMAGE_TYPE_1D;
	case GpuImageDimension::d2D:
		return VK_IMAGE_TYPE_2D;
	case GpuImageDimension::d3D:
		return VK_IMAGE_TYPE_3D;
	}

	OSK_ASSERT(false, UnreachableException("GpuImageDimension no reconocido."));
}

template <VulkanTarget Target>
VkImageViewType GpuImageVk<Target>::GetVkImageViewType() const {
	// Es cubo
	if (EFTraits::HasFlag(GetUsage(), GpuImageUsage::CUBEMAP))
		return VK_IMAGE_VIEW_TYPE_CUBE;

	switch (GetDimension()) {
	case GpuImageDimension::d1D:
		return VK_IMAGE_VIEW_TYPE_1D;
	case GpuImageDimension::d2D:
		return VK_IMAGE_VIEW_TYPE_2D;
	case GpuImageDimension::d3D:
		return VK_IMAGE_VIEW_TYPE_3D;
	}

	OSK_ASSERT(false, UnreachableException("GpuImageDimension no reconocido."));
}

template <VulkanTarget Target>
VkImageViewType GpuImageVk<Target>::GetVkImageArrayViewType() const {
	switch (GetDimension()) {
	case GpuImageDimension::d1D:
		return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
	case GpuImageDimension::d2D:
		return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

	default:
		break;
	}

	OSK_ASSERT(false, UnreachableException("GpuImageDimension no reconocido para ARRAY."));
}

#endif
