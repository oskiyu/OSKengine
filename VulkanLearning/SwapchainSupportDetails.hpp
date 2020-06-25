#pragma once

#include <vulkan/vulkan.h>

#include <vector>

struct SwapchainSupportDetails {
	//Capacidades de la superficie.
	VkSurfaceCapabilitiesKHR SurfaceCapabilities;

	//Formatos.
	std::vector<VkSurfaceFormatKHR> Formats;

	//Modos de presentaci�n.
	std::vector<VkPresentModeKHR> PresentModes;
};