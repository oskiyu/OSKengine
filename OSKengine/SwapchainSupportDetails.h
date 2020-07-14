#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace OSK::VULKAN {

	struct OSKAPI_CALL SwapchainSupportDetails {
		//Capacidades de la superficie.
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;

		//Formatos.
		std::vector<VkSurfaceFormatKHR> Formats;

		//Modos de presentación.
		std::vector<VkPresentModeKHR> PresentModes;
	};

}
