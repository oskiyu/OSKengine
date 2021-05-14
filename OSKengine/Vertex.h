#pragma once

#include <vulkan/vulkan.h>
#include <glm.hpp>
#include <array>

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

/// <summary>
/// N�mero m�ximo de huesos de un modelo animado.
/// </summary>
#define OSK_ANIM_MAX_BONES 64

/// <summary>
/// N�mero m�ximo de huesos que pueden modificar un v�rtice.
/// </summary>
#define OSK_ANIM_MAX_BONES_PER_VERTEX 4

/// <summary>
/// N�mero de atributos de un v�rtice.
/// </summary>
#define OSK_VERTEX_ATTRIBUTES_COUNT 6

namespace OSK {

	/// <summary>
	/// Representa un v�rtice 3D.
	/// </summary>
	struct OSKAPI_CALL Vertex {
		
		/// <summary>
		/// Posici�n en el mundo 3D.
		/// </summary>
		glm::vec3 position;
		
		/// <summary>
		/// Color.
		/// </summary>
		glm::vec3 color;
		
		/// <summary>
		/// Coordenadas de textura.
		/// </summary>
		glm::vec2 textureCoordinates;

		/// <summary>
		/// Vector normal.
		/// </summary>
		glm::vec3 normals;

		/// <summary>
		/// Peso de los huesos de un v�rtice animado.
		/// </summary>
		float_t boneWeights[OSK_ANIM_MAX_BONES_PER_VERTEX]{ 1.0f };

		/// <summary>
		/// IDs de los huesos de un modelo 3D animado.
		/// </summary>
		uint32_t bondeIDs[OSK_ANIM_MAX_BONES_PER_VERTEX]{ 0 };

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
