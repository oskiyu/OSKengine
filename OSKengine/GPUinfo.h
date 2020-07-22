#pragma once

#include <vulkan/vulkan.h>

#include "QueueFamilyIndices.h"
#include "SwapchainSupportDetails.h"

namespace OSK::VULKAN {

	//Informaci�n de una GPU.
	struct OSKAPI_CALL GPUinfo {
		VkPhysicalDeviceProperties Properties;
		VkPhysicalDeviceFeatures Features;
		QueueFamilyIndices Families;
		SwapchainSupportDetails ShapchainSupport;
		VkPhysicalDevice GPU;
		bool IsSuitable;
	};

}