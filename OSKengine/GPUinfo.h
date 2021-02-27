#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include "QueueFamilyIndices.h"
#include "SwapchainSupportDetails.h"

namespace OSK::VULKAN {

	//Información de una GPU.
	struct OSKAPI_CALL GPUinfo {
		VkPhysicalDeviceProperties Properties;
		VkPhysicalDeviceFeatures Features;
		QueueFamilyIndices Families;
		SwapchainSupportDetails ShapchainSupport;
		VkPhysicalDevice GPU;
		size_t minAlignment;
		bool IsSuitable;
	};

}