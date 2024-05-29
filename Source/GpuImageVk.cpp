#include "GpuImageVk.h"

#include "OSKengine.h"
#include "RendererVk.h"
#include "GpuVk.h"
#include "Logger.h"

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


VkFilter GpuImageVk::GetFilterTypeVulkan(GpuImageFilteringType type) {
	switch (type) {
	case OSK::GRAPHICS::GpuImageFilteringType::LIENAR:
		return VK_FILTER_LINEAR;
	case OSK::GRAPHICS::GpuImageFilteringType::NEAREST:
		return VK_FILTER_NEAREST;
	case OSK::GRAPHICS::GpuImageFilteringType::CUBIC:
		return VK_FILTER_CUBIC_IMG;
	}
}

VkSamplerAddressMode GpuImageVk::GetAddressModeVulkan(GpuImageAddressMode mode) {
	switch (mode) {
	case OSK::GRAPHICS::GpuImageAddressMode::REPEAT:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	case OSK::GRAPHICS::GpuImageAddressMode::MIRRORED_REPEAT:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	case OSK::GRAPHICS::GpuImageAddressMode::EDGE:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case OSK::GRAPHICS::GpuImageAddressMode::BACKGROUND_BLACK:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	case OSK::GRAPHICS::GpuImageAddressMode::BACKGROUND_WHITE:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	}
}

VkImageAspectFlags GpuImageVk::GetAspectFlags(SampledChannel channel) {
	VkImageAspectFlags aspectMask = 0;

	if (EFTraits::HasFlag(channel, SampledChannel::COLOR))
		aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
	if (EFTraits::HasFlag(channel, SampledChannel::DEPTH))
		aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
	if (EFTraits::HasFlag(channel, SampledChannel::STENCIL))
		aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

	return aspectMask;
}


GpuImageVk::GpuImageVk(const GpuImageCreateInfo& info, const ICommandQueue* ownerQueue) : GpuImage(info, ownerQueue) {

}

GpuImageVk::~GpuImageVk() {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	if (m_image != VK_NULL_HANDLE)
		vkDestroyImage(logicalDevice, m_image, nullptr);

	if (m_sampler != VK_NULL_HANDLE)
		vkDestroySampler(logicalDevice, m_sampler, nullptr);
}


/* VULKAN SPECIFICS */

void GpuImageVk::CreateVkImage() {
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
		queueIndices.insert(renderer->GetTransferOnlyQueue()->GetFamily().familyIndex);
	}
	else {
		if (GetOwnerQueue()->GetQueueType() == GpuQueueType::PRESENTATION) {
			if (renderer->UseUnifiedCommandQueue()) {
				queueIndices.insert(renderer->GetUnifiedQueue()->GetFamily().familyIndex);
			}
			else {
				queueIndices.insert(renderer->GetGraphicsComputeQueue()->GetFamily().familyIndex);
				queueIndices.insert(renderer->GetPresentationQueue()->GetFamily().familyIndex);
			}
		}

		if (GetOwnerQueue()->GetQueueType() == GpuQueueType::MAIN) {
			if (renderer->UseUnifiedCommandQueue()) {
				queueIndices.insert(renderer->GetUnifiedQueue()->GetFamily().familyIndex);
			}
			else {
				queueIndices.insert(renderer->GetGraphicsComputeQueue()->GetFamily().familyIndex);
			}
		}
	}

	DynamicArray<USize32> indices = DynamicArray<USize32>::CreateReservedArray(static_cast<USize32>(queueIndices.size()));
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
	
	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
	const VkResult result = vkCreateImage(device,	&imageInfo, nullptr, &m_image);

	OSK_ASSERT(result == VK_SUCCESS, ImageCreationException(result));
}

VkImage GpuImageVk::GetVkImage() const {
	return m_image;
}

void GpuImageVk::CreateVkSampler(const GpuImageSamplerDesc& samplerDesc) {
	//Info del sampler.
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	//Filtro:
	//	VK_FILTER_LINEAR: suavizado.
	//	VK_FILTER_NEAREST: pixelado.
	samplerInfo.minFilter = GetFilterTypeVulkan(samplerDesc.filteringType);
	samplerInfo.magFilter = GetFilterTypeVulkan(samplerDesc.filteringType);
	//AddressMode: como se accede a la imagen con TexCoords fuera de los límites.
	samplerInfo.addressModeU = GetAddressModeVulkan(samplerDesc.addressMode);
	samplerInfo.addressModeV = GetAddressModeVulkan(samplerDesc.addressMode);
	samplerInfo.addressModeW = GetAddressModeVulkan(samplerDesc.addressMode);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16.0f;

	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	if (samplerDesc.addressMode == GpuImageAddressMode::BACKGROUND_WHITE)
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	switch (samplerDesc.mipMapMode) {
	case GpuImageMipmapMode::AUTO:
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(GetMipLevels());
		break;

	case GpuImageMipmapMode::CUSTOM:
		samplerInfo.minLod = static_cast<float>(samplerDesc.minMipLevel);
		samplerInfo.maxLod = static_cast<float>(samplerDesc.maxMipLevel);
		break;

	case GpuImageMipmapMode::NONE:
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		break;
	}

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = -0.5f;

	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
	vkCreateSampler(device,	&samplerInfo, nullptr, &m_sampler);
}

VkSampler GpuImageVk::GetVkSampler() const {
	return m_sampler;
}

void GpuImageVk::_SetVkImage(VkImage img) {
	m_image = img;
}

void GpuImageVk::SetSwapchainView(VkImageView view) {
	m_swapchainView = view;
}

VkImageView GpuImageVk::GetSwapchainView() const {
	return m_swapchainView;
}

OwnedPtr<IGpuImageView> GpuImageVk::CreateView(const GpuImageViewConfig& viewConfig) const {
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

	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
	const VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &view);

	OSK_ASSERT(result == VK_SUCCESS, ImageViewCreationException(result));

	return new GpuImageViewVk(view, *this, viewConfig);
}

void GpuImageVk::SetDebugName(const std::string& name) {
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = VK_OBJECT_TYPE_IMAGE;
	nameInfo.pNext = nullptr;

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	nameInfo.pObjectName = name.c_str();
	nameInfo.objectHandle = (uint64_t)m_image;

	if (RendererVk::pvkSetDebugUtilsObjectNameEXT != nullptr)
		RendererVk::pvkSetDebugUtilsObjectNameEXT(logicalDevice, &nameInfo);

	m_name = name;
}


VkImageType GpuImageVk::GetVkImageType() const {
	switch (GetDimension()) {
	case GpuImageDimension::d1D:
		return VK_IMAGE_TYPE_1D;
	case GpuImageDimension::d2D:
		return VK_IMAGE_TYPE_2D;
	case GpuImageDimension::d3D:
		return VK_IMAGE_TYPE_3D;
	}
}

VkImageViewType GpuImageVk::GetVkImageViewType() const {
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
}

VkImageViewType GpuImageVk::GetVkImageArrayViewType() const {
	switch (GetDimension()) {
	case GpuImageDimension::d1D:
		return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
	case GpuImageDimension::d2D:
		return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	}
}
