#pragma once

#include <vulkan/vulkan.h>
#include <glm.hpp>
#include <array>

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

/// <summary>
/// Número máximo de huesos de un modelo animado.
/// </summary>
#define OSK_ANIM_MAX_BONES 64

/// <summary>
/// Número máximo de huesos que pueden modificar un vértice.
/// </summary>
#define OSK_ANIM_MAX_BONES_PER_VERTEX 4

/// <summary>
/// Número de atributos de un vértice.
/// </summary>
#define OSK_VERTEX_ATTRIBUTES_COUNT 6

namespace OSK {

	/// <summary>
	/// Representa un vértice 3D.
	/// </summary>
	struct OSKAPI_CALL Vertex {
		
		/// <summary>
		/// Posición en el mundo 3D.
		/// </summary>
		glm::vec3 Position;
		
		/// <summary>
		/// Color.
		/// </summary>
		glm::vec3 Color;
		
		/// <summary>
		/// Coordenadas de textura.
		/// </summary>
		glm::vec2 TextureCoordinates;

		/// <summary>
		/// Vector normal.
		/// </summary>
		glm::vec3 Normals;

		/// <summary>
		/// Peso de los huesos de un vértice animado.
		/// </summary>
		float_t BoneWeights[OSK_ANIM_MAX_BONES_PER_VERTEX]{ 1.0f };

		/// <summary>
		/// IDs de los huesos de un modelo 3D animado.
		/// </summary>
		uint32_t BondeIDs[OSK_ANIM_MAX_BONES_PER_VERTEX]{ 0 };

		//Atributos que se pasan a la GPU.

		/// <summary>
		/// Atributos que se pasan a la GPU.
		/// </summary>
		/// <returns>Atributos que se pasan a la GPU.</returns>
		static VkVertexInputBindingDescription GetBindingDescription();

		/// <summary>
		/// Atributos que se pasan a la GPU.
		/// </summary>
		/// <returns>Atributos que se pasan a la GPU.</returns>
		static std::array<VkVertexInputAttributeDescription, OSK_VERTEX_ATTRIBUTES_COUNT> GetAttributeDescriptions_FullVertex();

	};

}
