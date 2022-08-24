#pragma once

#include "IGpuImageView.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuImageViewVulkan : public IGpuImageView {

	public:

		GpuImageViewVulkan(VkImageView view, SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage);
		~GpuImageViewVulkan();

		VkImageView GetVkView() const;

	private:

		VkImageView view = VK_NULL_HANDLE;

	};

}
