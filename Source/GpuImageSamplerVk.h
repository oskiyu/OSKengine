#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IGpuImageSampler.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuVk;

	class GpuImageSamplerVk final : public IGpuImageSampler {

	public:

		explicit GpuImageSamplerVk(
			const GpuImageSamplerDesc& info,
			const GpuVk* gpu);
		~GpuImageSamplerVk() override;

		VkSampler GetSamplerVk() const;

		static VkFilter GetFilterTypeVk(GpuImageFilteringType type);

	private:

		static VkSamplerAddressMode GetAddressModeVk(GpuImageAddressMode mode);
		static VkSampler CreateSamplerVk(
			const GpuImageSamplerDesc& info,
			const GpuVk* gpu);

		VkSampler m_sampler = VK_NULL_HANDLE;

		const GpuVk* m_gpu = nullptr;

	};

}

#endif
