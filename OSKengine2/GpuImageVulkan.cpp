#include "GpuImageVulkan.h"

#include "OSKengine.h"
#include "RendererVulkan.h"
#include "GpuVulkan.h"
#include "Logger.h"

#include "FormatVulkan.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypeVulkan.h"
#include "GpuImageDimensions.h"
#include "CommandQueueVulkan.h"
#include "GpuImageViewVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageVulkan::GpuImageVulkan(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format, TSize numSamples, GpuImageSamplerDesc samplerDesc)
	: GpuImage(size, dimension, usage, numLayers, format, numSamples, samplerDesc) {

}

GpuImageVulkan::~GpuImageVulkan() {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();

	if (image != VK_NULL_HANDLE)
		vkDestroyImage(logicalDevice, image, nullptr);

	if (sampler != VK_NULL_HANDLE)
		vkDestroySampler(logicalDevice, sampler, nullptr);
}


/* VULKAN SPECIFICS */

void GpuImageVulkan::CreateVkImage() {
	bool usesMultipleQueues = EFTraits::HasFlag(GetUsage(), GpuImageUsage::COMPUTE) && GetUsage() != GpuImageUsage::COMPUTE;

	VkImageCreateInfo imageInfo{};
	
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = GetVkImageType();

	imageInfo.extent.width = GetSize().X;
	imageInfo.extent.height = GetSize().Y;
	imageInfo.extent.depth = GetSize().Z;

	imageInfo.mipLevels = GetMipLevels();
	imageInfo.arrayLayers = GetNumLayers();

	imageInfo.format = GetFormatVulkan(GetFormat());
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = GetGpuImageUsageVulkan(GetUsage());
	imageInfo.samples = (VkSampleCountFlagBits)GetNumSamples();

	DynamicArray<uint32_t> queueIndices;
	if (usesMultipleQueues) {
		queueIndices.Insert(Engine::GetRenderer()->GetGraphicsCommandQueue()->As<CommandQueueVulkan>()->GetQueueIndex());
		queueIndices.Insert(Engine::GetRenderer()->GetComputeCommandQueue()->As<CommandQueueVulkan>()->GetQueueIndex());

		if (queueIndices[0] == queueIndices[1])
			usesMultipleQueues = false;
	}
	else if (GetUsage() == GpuImageUsage::COMPUTE) {
		queueIndices.Insert(Engine::GetRenderer()->GetComputeCommandQueue()->As<CommandQueueVulkan>()->GetQueueIndex());
	}
	else {
		queueIndices.Insert(Engine::GetRenderer()->GetGraphicsCommandQueue()->As<CommandQueueVulkan>()->GetQueueIndex());
	}

	imageInfo.queueFamilyIndexCount = usesMultipleQueues ? queueIndices.GetSize() : 1;
	imageInfo.pQueueFamilyIndices = queueIndices.GetData();
	imageInfo.sharingMode = usesMultipleQueues
		? VK_SHARING_MODE_CONCURRENT
		: VK_SHARING_MODE_EXCLUSIVE;

	// Si la imagen se usará como cubemap, debemos especificarlo.
	imageInfo.flags = EFTraits::HasFlag(GetUsage(), GpuImageUsage::CUBEMAP) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	
	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
	VkResult result = vkCreateImage(device,	&imageInfo, nullptr, &image);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear la imagen en la GPU.");
}

VkImage GpuImageVulkan::GetVkImage() const {
	return image;
}

void GpuImageVulkan::CreateVkSampler(const GpuImageSamplerDesc& samplerDesc) {
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

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	if (samplerDesc.addressMode == GpuImageAddressMode::BACKGROUND_WHITE)
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;

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
	samplerInfo.mipLodBias = 0.0f;

	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
	vkCreateSampler(device,	&samplerInfo, nullptr, &sampler);
}

VkSampler GpuImageVulkan::GetVkSampler() const {
	return sampler;
}

void GpuImageVulkan::_SetVkImage(VkImage img) {
	image = img;
}

void GpuImageVulkan::SetSwapchainView(VkImageView view) {
	swapchainView = view;
}

VkImageView GpuImageVulkan::GetSwapchainView() const {
	return swapchainView;
}

OwnedPtr<IGpuImageView> GpuImageVulkan::CreateView(SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage) const {
	VkImageView view = VK_NULL_HANDLE;
	
	VkImageViewCreateInfo viewInfo{};

	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = arrayType == SampledArrayType::SINGLE_LAYER ? GetVkImageViewType() : GetVkImageArrayViewType();
	viewInfo.format = GetFormatVulkan(GetFormat());
	
	VkImageAspectFlags aspectMask = 0;
	if (EFTraits::HasFlag(channel, SampledChannel::COLOR))
		aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
	if (EFTraits::HasFlag(channel, SampledChannel::DEPTH))
		aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
	if (EFTraits::HasFlag(channel, SampledChannel::STENCIL))
		aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

	viewInfo.subresourceRange.aspectMask = aspectMask;

	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = usage == ViewUsage::COLOR_TARGET ? 1 : GetMipLevels();

	viewInfo.subresourceRange.baseArrayLayer = baseArrayLevel;
	viewInfo.subresourceRange.layerCount = layerCount;

	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
	VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &view);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el view de stencil.");

	return new GpuImageViewVulkan(view, channel, arrayType, baseArrayLevel, layerCount, usage);
}

void GpuImageVulkan::SetDebugName(const std::string& name) {
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = VK_OBJECT_TYPE_IMAGE;
	nameInfo.pNext = nullptr;

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();

	nameInfo.pObjectName = name.c_str();
	nameInfo.objectHandle = (uint64_t)image;

	if (RendererVulkan::pvkSetDebugUtilsObjectNameEXT != nullptr)
		RendererVulkan::pvkSetDebugUtilsObjectNameEXT(logicalDevice, &nameInfo);
}


VkImageType GpuImageVulkan::GetVkImageType() const {
	switch (GetDimension()) {
	case GpuImageDimension::d1D:
		return VK_IMAGE_TYPE_1D;
	case GpuImageDimension::d2D:
		return VK_IMAGE_TYPE_2D;
	case GpuImageDimension::d3D:
		return VK_IMAGE_TYPE_3D;
	}
}

VkImageViewType GpuImageVulkan::GetVkImageViewType() const {
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

VkImageViewType GpuImageVulkan::GetVkImageArrayViewType() const {
	switch (GetDimension()) {
	case GpuImageDimension::d1D:
		return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
	case GpuImageDimension::d2D:
		return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	}
}

VkFilter GpuImageVulkan::GetFilterTypeVulkan(GpuImageFilteringType type) {
	switch (type) {
	case OSK::GRAPHICS::GpuImageFilteringType::LIENAR:
		return VK_FILTER_LINEAR;
	case OSK::GRAPHICS::GpuImageFilteringType::NEAREST:
		return VK_FILTER_NEAREST;
	case OSK::GRAPHICS::GpuImageFilteringType::CUBIC:
		return VK_FILTER_CUBIC_IMG;
	default:
		return VK_FILTER_LINEAR;
	}
}

VkSamplerAddressMode GpuImageVulkan::GetAddressModeVulkan(GpuImageAddressMode mode) {
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
	default:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}
}
