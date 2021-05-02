#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include "QueueFamilyIndices.h"
#include "SwapchainSupportDetails.h"

namespace OSK::VULKAN {

	/// <summary>
	/// informaci�n interna de una GPU.
	/// </summary>
	struct OSKAPI_CALL GPUinfo {

		/// <summary>
		/// Propiedades f�sicas.
		/// </summary>
		VkPhysicalDeviceProperties Properties;

		/// <summary>
		/// Cosas que soporta.
		/// </summary>
		VkPhysicalDeviceFeatures Features;

		/// <summary>
		/// Familias de comandos que tiene.
		/// </summary>
		QueueFamilyIndices Families;

		/// <summary>
		/// Detalles del soporte de swapchain.
		/// </summary>
		SwapchainSupportDetails ShapchainSupport;

		/// <summary>
		/// GPU.
		/// </summary>
		VkPhysicalDevice GPU;

		/// <summary>
		/// Alineamiento m�nimo para dynamic ubos.
		/// </summary>
		size_t minAlignment;

		/// <summary>
		/// True si puede usarse en OSKengine.
		/// </summary>
		bool IsSuitable;

	};

}