#include "GpuImageVulkan.h"

#include "OSKengine.h"
#include "RendererVulkan.h"
#include "GpuVulkan.h"
#include "Logger.h"

#include "FormatVulkan.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypeVulkan.h"
#include "GpuImageDimensions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageVulkan::GpuImageVulkan(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format, TSize numSamples, GpuImageSamplerDesc samplerDesc)
	: GpuImage(size, dimension, usage, numLayers, format, numSamples, samplerDesc) {

}

GpuImageVulkan::~GpuImageVulkan() {
	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();

	if (image != VK_NULL_HANDLE)
		vkDestroyImage(logicalDevice, image, nullptr);

	for (auto view : colorViews)
		if (view != VK_NULL_HANDLE)
			vkDestroyImageView(logicalDevice, view, nullptr);

	for (auto view : depthStencilViews)
		if (view != VK_NULL_HANDLE)
			vkDestroyImageView(logicalDevice, view, nullptr);

	for (auto view : depthViews)
		if (view != VK_NULL_HANDLE)
			vkDestroyImageView(logicalDevice, view, nullptr);

	for (auto view : stencilViews)
		if (view != VK_NULL_HANDLE)
			vkDestroyImageView(logicalDevice, view, nullptr);

	if (arrayColorView != VK_NULL_HANDLE)
		vkDestroyImageView(logicalDevice, arrayColorView, nullptr);
	if (arrayDepthView != VK_NULL_HANDLE)
		vkDestroyImageView(logicalDevice, arrayDepthView, nullptr);
	if (arrayStencilView != VK_NULL_HANDLE)
		vkDestroyImageView(logicalDevice, arrayStencilView, nullptr);

	if (sampler != VK_NULL_HANDLE)
		vkDestroySampler(logicalDevice, sampler, nullptr);
}


/* VULKAN SPECIFICS */

void GpuImageVulkan::CreateVkImage() {
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
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = (VkSampleCountFlagBits)GetNumSamples();

	// Si la imagen se usará como cubemap, debemos especificarlo.
	imageInfo.flags = EFTraits::HasFlag(GetUsage(), GpuImageUsage::CUBEMAP) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
	VkResult result = vkCreateImage(device,	&imageInfo, nullptr, &image);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear la imagen en la GPU.");
}

VkImage GpuImageVulkan::GetVkImage() const {
	return image;
}


void GpuImageVulkan::CreateColorArrayView() {
	OSK_ASSERT(image != VK_NULL_HANDLE, "Se debe crear la propia imagen antes de crear el view.");
	OSK_ASSERT(EFTraits::HasFlag(GetUsage(), GpuImageUsage::COLOR), "La imagen debe haberse creado con GpuImageUsage::COLOR.");

	VkImageViewCreateInfo viewInfo{};

	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = GetVkImageArrayViewType();
	viewInfo.format = GetFormatVulkan(GetFormat());

	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = GetMipLevels();

	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = GetNumLayers();

	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
	VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &arrayColorView);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el view por defecto.");
}

VkImageView GpuImageVulkan::GetColorArrayView() const {
	return arrayColorView;
}

void GpuImageVulkan::CreateColorViews() {
	OSK_ASSERT(image != VK_NULL_HANDLE, "Se debe crear la propia imagen antes de crear el view.");

	const bool isCubemap = EFTraits::HasFlag(GetUsage(), GpuImageUsage::CUBEMAP);

	for (TSize i = 0; i < GetNumLayers(); i++) {
		VkImageView view = VK_NULL_HANDLE;

		VkImageViewCreateInfo viewInfo{};

		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = GetVkImageViewType();
		viewInfo.format = GetFormatVulkan(GetFormat());

		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = GetMipLevels();

		viewInfo.subresourceRange.baseArrayLayer = i;
		viewInfo.subresourceRange.layerCount = isCubemap ? 6 : 1;

		const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
		VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &view);
		OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el view por defecto.");

		colorViews.Insert(view);

		if (EFTraits::HasFlag(GetUsage(), GpuImageUsage::CUBEMAP))
			i += 5;
	}
}

VkImageView GpuImageVulkan::GetColorView(TSize arrayLevel) const {
	return colorViews.At(arrayLevel);
}

void GpuImageVulkan::CreateDepthStencilViews() {
	OSK_ASSERT(image != VK_NULL_HANDLE, "Se debe crear la propia imagen antes de crear el view.");
	OSK_ASSERT(EFTraits::HasFlag(GetUsage(), GpuImageUsage::DEPTH_STENCIL), "La imagen debe haberse creado con GpuImageUsage::DEPTH_STENCIL.");

	for (TSize i = 0; i < GetNumLayers(); i++) {
		VkImageView view = VK_NULL_HANDLE;

		VkImageViewCreateInfo viewInfo{};

		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = GetVkImageViewType();
		viewInfo.format = GetFormatVulkan(GetFormat());

		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = GetMipLevels();

		viewInfo.subresourceRange.baseArrayLayer = i;
		viewInfo.subresourceRange.layerCount = 1;

		const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
		VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &view);
		OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el view por defecto.");

		depthStencilViews.Insert(view);
	}
}

VkImageView GpuImageVulkan::GetDepthStencilView(TSize arrayLevel) const {
	return depthStencilViews.At(arrayLevel);
}

void GpuImageVulkan::CreateDepthArrayView() {
	OSK_ASSERT(image != VK_NULL_HANDLE, "Se debe crear la propia imagen antes de crear el view.");
	OSK_ASSERT(EFTraits::HasFlag(GetUsage(), GpuImageUsage::DEPTH_STENCIL), "La imagen debe haberse creado con GpuImageUsage::DEPTH_STENCIL.");

	VkImageViewCreateInfo viewInfo{};

	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = GetVkImageArrayViewType();
	viewInfo.format = GetFormatVulkan(GetFormat());

	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = GetMipLevels();

	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = GetNumLayers();

	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
	VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &arrayDepthView);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el view por defecto.");
}

VkImageView GpuImageVulkan::GetDepthArrayView() const {
	return arrayDepthView;
}

void GpuImageVulkan::CreateDepthOnlyViews() {
	OSK_ASSERT(image != VK_NULL_HANDLE, "Se debe crear la propia imagen antes de crear el view.");
	OSK_ASSERT(EFTraits::HasFlag(GetUsage(), GpuImageUsage::DEPTH_STENCIL), "La imagen debe haberse creado con GpuImageUsage::DEPTH_STENCIL.");

	for (TSize i = 0; i < GetNumLayers(); i++) {
		VkImageView view = VK_NULL_HANDLE;

		VkImageViewCreateInfo viewInfo{};

		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = GetVkImageViewType();
		viewInfo.format = GetFormatVulkan(GetFormat());

		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = GetMipLevels();

		viewInfo.subresourceRange.baseArrayLayer = i;
		viewInfo.subresourceRange.layerCount = 1;

		const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
		VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &view);
		OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el view de profundidad.");

		depthViews.Insert(view);
	}
}

VkImageView GpuImageVulkan::GetDepthOnlyView(TSize arrayLevel) const {
	return depthViews.At(arrayLevel);
}

void GpuImageVulkan::CreateStencilArrayView() {
	OSK_ASSERT(image != VK_NULL_HANDLE, "Se debe crear la propia imagen antes de crear el view.");
	OSK_ASSERT(EFTraits::HasFlag(GetUsage(), GpuImageUsage::DEPTH_STENCIL), "La imagen debe haberse creado con GpuImageUsage::DEPTH_STENCIL.");

	VkImageViewCreateInfo viewInfo{};

	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = GetVkImageArrayViewType();
	viewInfo.format = GetFormatVulkan(GetFormat());

	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;

	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = GetMipLevels();

	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = GetNumLayers();

	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
	VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &arrayStencilView);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el view por defecto.");
}

VkImageView GpuImageVulkan::GetStencilArrayView() const {
	return arrayStencilView;
}

void GpuImageVulkan::CreateStencilOnlyViews() {
	OSK_ASSERT(image != VK_NULL_HANDLE, "Se debe crear la propia imagen antes de crear el view.");
	OSK_ASSERT(EFTraits::HasFlag(GetUsage(), GpuImageUsage::DEPTH_STENCIL), "La imagen debe haberse creado con GpuImageUsage::DEPTH_STENCIL.");

	for (TSize i = 0; i < GetNumLayers(); i++) {
		VkImageView view = VK_NULL_HANDLE;

		VkImageViewCreateInfo viewInfo{};

		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = GetVkImageViewType();
		viewInfo.format = GetFormatVulkan(GetFormat());

		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;

		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = GetMipLevels();

		viewInfo.subresourceRange.baseArrayLayer = i;
		viewInfo.subresourceRange.layerCount = 1;

		const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
		VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &view);
		OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el view de stencil.");

		stencilViews.Insert(view);
	}
}

VkImageView GpuImageVulkan::GetStencilOnlyView(TSize arrayLevel) const {
	return stencilViews.At(arrayLevel);
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

void GpuImageVulkan::_SetView(VkImageView view) {
	if (colorViews.IsEmpty())
		colorViews.Insert(view);
	else
		colorViews[0] = view;
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
