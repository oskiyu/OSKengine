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
	struct OSKAPI_CALL GpuInfo {

		/// <summary>
		/// Propiedades f�sicas.
		/// </summary>
		VkPhysicalDeviceProperties properties;

		/// <summary>
		/// Cosas que soporta.
		/// </summary>
		VkPhysicalDeviceFeatures features;

		/// <summary>
		/// Familias de comandos que tiene.
		/// </summary>
		QueueFamilyIndices families;

		/// <summary>
		/// Detalles del soporte de swapchain.
		/// </summary>
		SwapchainSupportDetails shapchainSupport;

		/// <summary>
		/// GPU.
		/// </summary>
		VkPhysicalDevice gpu;

		/// <summary>
		/// Alineamiento m�nimo para dynamic ubos.
		/// </summary>
		size_t minAlignment;

		/// <summary>
		/// True si puede usarse en OSKengine.
		/// </summary>
		bool isSuitable;

	};

}