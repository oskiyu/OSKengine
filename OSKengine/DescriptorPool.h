#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include "DynamicArray.hpp"

namespace OSK {

	class DescriptorLayout;

	class OSKAPI_CALL DescriptorPool {

		friend class RenderAPI;

	public:

		DescriptorPool(VkDevice logicalDevice, uint32_t shapchainImageCount) : LogicalDevice(logicalDevice), SwapchainImageCount(shapchainImageCount) {

		}

		~DescriptorPool();

		void AddBinding(VkDescriptorType type);

		void SetLayout(DescriptorLayout* layout);

		void Create(uint32_t maxSets);
		
		DynamicArray<VkDescriptorPoolSize> DescriptorPoolSizes;

		VkDescriptorPool VulkanDescriptorPool = VK_NULL_HANDLE;

		uint32_t SwapchainImageCount;
		VkDevice LogicalDevice;

	};

}