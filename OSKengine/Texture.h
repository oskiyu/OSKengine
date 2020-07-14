#pragma once

#include "VulkanImage.h"
#include <vector>

namespace OSK {

	class VulkanRenderer;

	struct OSKAPI_CALL Texture {
		friend class VulkanRenderer;

	private:
		VULKAN::VulkanImage image{};

		std::vector<VkDescriptorSet> DescriptorSets{};

	};

}
