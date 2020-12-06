#pragma once

#include "OSKmacros.h"

#include <vulkan/vulkan.h>

#include "PointLight.h"
#include "DirectionalLight.h"

#include <vector>
#include "VulkanBuffer.h"

namespace OSK {

	//Interfaz con un UBO que contiene información sobre las luces.
	class OSKAPI_CALL LightUBO {

	public:

		//Luz direccional.
		DirectionalLight Directional{};

		//Luces puntuales.
		std::vector<PointLight> Points{};

		//Obtiene el tamaño, en bytes, de todas las luces puntuales juntas.
		inline const size_t PointsSize() const {
			return Points.size() * sizeof(PointLight);
		}

		//Obtiene el tamaño, en bytes, del UBO.
		inline const size_t Size() const {
			return sizeof(DirectionalLight) + PointsSize();
		}

		//Actualiza el UBO.
		//	<logicalDevice>: logicalDevice.
		//	<buffer>: Vulkan buffer donde se almacena el UBO.
		void UpdateBuffer(VkDevice logicalDevice, VulkanBuffer& buffer);

	};

}