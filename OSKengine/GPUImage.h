#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

namespace OSK::VULKAN {

	/// <summary>
	/// Imagen almacenada en la GPU.
	/// </summary>
	struct OSKAPI_CALL GPUImage {

		/// <summary>
		/// Imagen nativa.
		/// </summary>
		VkImage image = VK_NULL_HANDLE;

		/// <summary>
		/// Memoria en la que se guarda la imagen.
		/// </summary>
		VkDeviceMemory memory = VK_NULL_HANDLE;

		/// <summary>
		/// Cómo se accede a la imagen.
		/// </summary>
		VkImageView view = VK_NULL_HANDLE;

		//Sampler

		/// <summary>
		/// Sampler.
		/// </summary>
		VkSampler sampler = VK_NULL_HANDLE;

		/// <summary>
		/// Logical device del renderizador.
		/// </summary>
		VkDevice* logicalDevice = nullptr;

		/// <summary>
		/// Elimina la imagen.
		/// </summary>
		void Destroy();

	};

}