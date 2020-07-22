#pragma once

#include <vulkan/vulkan.h>
#include "OSKmacros.h"

namespace OSK::VULKAN {

	//Imagen de Vulkan.
	struct OSKAPI_CALL VulkanImage {
		//Informaci�n de la imagen.
		VkImage Image;
		//Memoria en la que se almacena la imagen.
		VkDeviceMemory Memory;
		//C�mo se accede a la imagen.
		VkImageView View;
	};

}