#pragma once

#include "IGpuImageView.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuImageViewVk final : public IGpuImageView {

	public:

		GpuImageViewVk(
			VkImageView view, 
			SampledChannel channel, 
			SampledArrayType arrayType, 
			TSize baseArrayLevel, 
			TSize layerCount, 
			ViewUsage usage);

		~GpuImageViewVk();

		VkImageView GetVkView() const;

	private:

		VkImageView view = VK_NULL_HANDLE;

	};

}
