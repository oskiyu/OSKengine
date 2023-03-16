#pragma once

#include "IGpuImageView.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuImageViewVk final : public IGpuImageView {

	public:

		GpuImageViewVk(
			VkImageView view,
			const GpuImage& image,
			const GpuImageViewConfig& config);

		~GpuImageViewVk();

		VkImageView GetVkView() const;

	private:

		VkImageView view = VK_NULL_HANDLE;

	};

}
