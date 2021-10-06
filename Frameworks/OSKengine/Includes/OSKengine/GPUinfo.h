#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include "QueueFamilyIndices.h"
#include "SwapchainSupportDetails.h"

namespace OSK {
	class RenderAPI;
}

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
		/// M�ximo nivel de MSAA disponible.
		/// </summary>
		VkSampleCountFlagBits maxMsaaSamples;

		/// <summary>
		/// Alineamiento m�nimo para dynamic ubos.
		/// </summary>
		size_t minAlignment;

		/// <summary>
		/// True si puede usarse en OSKengine.
		/// </summary>
		bool isSuitable;

		/// <summary>
		/// Actualiza el nivel m�ximo de MSAA, dentro de esta estructura.
		/// </summary>
		void SetMaxMsaa();

		/// <summary>
		/// Actualiza los detalles del swapchain.
		/// </summary>
		void UpdateSwapchainSupport();

		/// <summary>
		/// Renderizador.
		/// </summary>
		const RenderAPI* renderer = nullptr;

	};

}
