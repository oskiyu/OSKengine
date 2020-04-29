#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include "Log.h"
#define GLFW_INCLUDE_NONE
#include <glfw3.h>
#include "OSKsettings.h"

namespace OSK::VULKAN {

	struct VulkanInstance {

		VkInstance Instance;


		bool UseValidationLayers;


		void CreateInstance(const std::string& appName, const int32_t& appVersionMayor, const int32_t& appVersionMinor, const int32_t& appVersionRevision);


		bool CheckValidationLayerSupport();


		void Destroy();

		
		std::vector<const char*> GetRequiredExtensions();


		const std::vector<const char*> ValidationLayers{
			"VK_LAYER_KHRONOS_validation"
		};

	};

}