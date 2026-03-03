#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IGpuImageSampler.h"
#include "VulkanTarget.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	template <OSK::GRAPHICS::VulkanTarget> class GpuVk;

	template <VulkanTarget Target>
	class GpuImageSamplerVk final : public IGpuImageSampler {

	public:

		explicit GpuImageSamplerVk(
			const GpuImageSamplerDesc& info,
			const GpuVk<Target>* gpu);
		~GpuImageSamplerVk() override;

		VkSampler GetSamplerVk() const;

		static VkFilter GetFilterTypeVk(GpuImageFilteringType type);

	private:

		static VkSamplerAddressMode GetAddressModeVk(GpuImageAddressMode mode);
		static VkSampler CreateSamplerVk(
			const GpuImageSamplerDesc& info,
			const GpuVk<Target>* gpu);

		VkSampler m_sampler = VK_NULL_HANDLE;

		const GpuVk<Target>* m_gpu = nullptr;

	};

	template class GpuImageSamplerVk<VulkanTarget::VK_1_0>;
	template class GpuImageSamplerVk<VulkanTarget::VK_LATEST>;

}

#endif
