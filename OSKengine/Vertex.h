#pragma once

#include <vulkan/vulkan.h>
#include <glm.hpp>
#include <array>

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#define OSK_ANIM_MAX_BONES 64
#define OSK_ANIM_MAX_BONES_PER_VERTEX 4

#define OSK_VERTEX_ATTRIBUTES_COUNT 6

namespace OSK {

	//Vértice.
	struct OSKAPI_CALL Vertex {
		//Position.
		glm::vec3 Position;
		//Color.
		glm::vec3 Color;
		//Coordenadas.
		glm::vec2 TextureCoordinates;
		//Normales
		glm::vec3 Normals;

		float_t BoneWeights[OSK_ANIM_MAX_BONES_PER_VERTEX]{ 1.0f };
		uint32_t BondeIDs[OSK_ANIM_MAX_BONES_PER_VERTEX]{ 0 };

		//Atributos que se pasan a la GPU.
		static VkVertexInputBindingDescription GetBindingDescription();

		//Atributos que se pasan a la GPU.
		static std::array<VkVertexInputAttributeDescription, OSK_VERTEX_ATTRIBUTES_COUNT> GetAttributeDescriptions();

	};
}