#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

namespace OSK::VULKAN {

	//Imagen de Vulkan.
	struct OSKAPI_CALL VulkanImage {
		//Información de la imagen.
		VkImage Image = VK_NULL_HANDLE;
		//Memoria en la que se almacena la imagen.
		VkDeviceMemory Memory = VK_NULL_HANDLE;
		//Cómo se accede a la imagen.
		VkImageView View = VK_NULL_HANDLE;
		//Sampler
		VkSampler Sampler = VK_NULL_HANDLE;

		VkDevice* logicalDevice = nullptr;

		void Destroy();
	};

}