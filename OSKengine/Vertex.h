#pragma once

#include <vulkan/vulkan.h>
#include <glm.hpp>
#include <array>

#include "OSKmacros.h"

namespace OSK {

	struct OSKAPI_CALL Vertex {
		//Position.
		glm::vec3 Position;
		//Color.
		glm::vec3 Color;
		//Coordenadas.
		glm::vec2 TextureCoordinates;

		//Atributos que se pasan a la GPU.
		static VkVertexInputBindingDescription GetBindingDescription();

		//Atributos que se pasan a la GPU.
		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();

	};
}