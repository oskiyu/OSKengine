#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace OSK::VULKAN {

	/// <summary>
	/// Detalles sobre el soporte de swapchain de una GPU.
	/// </summary>
	struct OSKAPI_CALL SwapchainSupportDetails {

		/// <summary>
		/// Capacidades de la superficie.
		/// </summary>
		VkSurfaceCapabilitiesKHR surfaceCapabilities;

		/// <summary>
		/// Formatos soportados.
		/// </summary>
		std::vector<VkSurfaceFormatKHR> formats;

		/// <summary>
		/// Modos de presentación soportados.
		/// </summary>
		std::vector<VkPresentModeKHR> presentModes;

	};

}
