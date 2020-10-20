#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

namespace OSK::VULKAN {

	//Imagen de Vulkan.
	struct OSKAPI_CALL VulkanImage {
		//Informaci�n de la imagen.
		VkImage Image;
		//Memoria en la que se almacena la imagen.
		VkDeviceMemory Memory;
		//C�mo se accede a la imagen.
		VkImageView View;
		//Sampler
		VkSampler Sampler;

		VkDevice* logicalDevice;

		void Destroy();
	};

}