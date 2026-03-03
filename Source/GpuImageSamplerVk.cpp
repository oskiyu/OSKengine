#include "GpuImageSamplerVk.h"

#include "GpuVk.h"

#include "Assert.h"
#include "UnreachableException.h"

template <OSK::GRAPHICS::VulkanTarget Target>
OSK::GRAPHICS::GpuImageSamplerVk<Target>::GpuImageSamplerVk(const GpuImageSamplerDesc& info, const GpuVk<Target>* gpu)
	: IGpuImageSampler(info), m_sampler(CreateSamplerVk(info, gpu)), m_gpu(gpu) {

}

template <OSK::GRAPHICS::VulkanTarget Target>
OSK::GRAPHICS::GpuImageSamplerVk<Target>::~GpuImageSamplerVk() {
	vkDestroySampler(m_gpu->GetLogicalDevice(), m_sampler, nullptr);
}

template <OSK::GRAPHICS::VulkanTarget Target>
VkSampler OSK::GRAPHICS::GpuImageSamplerVk<Target>::GetSamplerVk() const {
	return m_sampler;
}

template <OSK::GRAPHICS::VulkanTarget Target>
VkSampler OSK::GRAPHICS::GpuImageSamplerVk<Target>::CreateSamplerVk(const GpuImageSamplerDesc& info, const GpuVk<Target>* gpu) {
	VkSampler output = VK_NULL_HANDLE;

	// Info del sampler.
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	// Filtro:
	samplerInfo.minFilter = GetFilterTypeVk(info.filteringType);
	samplerInfo.magFilter = GetFilterTypeVk(info.filteringType);

	// AddressMode: como se accede a la imagen con TexCoords fuera de los límites.
	samplerInfo.addressModeU = GetAddressModeVk(info.addressMode);
	samplerInfo.addressModeV = GetAddressModeVk(info.addressMode);
	samplerInfo.addressModeW = GetAddressModeVk(info.addressMode);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16.0f;

	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	if (info.addressMode == GpuImageAddressMode::BACKGROUND_WHITE) {
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	}

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	switch (info.mipMapMode) {
	case GpuImageMipmapMode::AUTO: // (preprocesado para tener el número de mips en info.maxMipLevel).
	case GpuImageMipmapMode::CUSTOM:
		samplerInfo.minLod = static_cast<float>(info.minMipLevel);
		samplerInfo.maxLod = static_cast<float>(info.maxMipLevel);
		break;

	case GpuImageMipmapMode::NONE:
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		break;
	}

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = -0.5f;

	const VkDevice device = gpu->GetLogicalDevice();
	vkCreateSampler(device, &samplerInfo, nullptr, &output);

	return output;
}

template <OSK::GRAPHICS::VulkanTarget Target>
VkFilter OSK::GRAPHICS::GpuImageSamplerVk<Target>::GetFilterTypeVk(GpuImageFilteringType type) {
	switch (type) {
	case OSK::GRAPHICS::GpuImageFilteringType::LINEAR:
		return VK_FILTER_LINEAR;
	case OSK::GRAPHICS::GpuImageFilteringType::NEAREST:
		return VK_FILTER_NEAREST;
	case OSK::GRAPHICS::GpuImageFilteringType::CUBIC:
		return VK_FILTER_CUBIC_IMG;
	}

	OSK_ASSERT(false, UnreachableException("GpuImageFilteringType no reconocido."));
}

template <OSK::GRAPHICS::VulkanTarget Target>
VkSamplerAddressMode OSK::GRAPHICS::GpuImageSamplerVk<Target>::GetAddressModeVk(GpuImageAddressMode mode) {
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

	OSK_ASSERT(false, UnreachableException("GpuImageAddressMode no reconocido."));
}
