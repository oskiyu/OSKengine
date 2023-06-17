#pragma once

#include "DynamicArray.hpp"

#include <string>
#include <glm/glm.hpp>

#include <limits>

namespace OSK::GRAPHICS {

	class Animator;

	/// @brief Indica cómo aplicar una animación a un modelo animado.
	struct AnimationSkin {

		/// @brief Nombre original de la animación.
		std::string name;


		/// @brief Índice de este nodo.
		UIndex32 thisIndex = std::numeric_limits<UIndex32>::max();

		/// @brief Índice del nodo  root de la skin.
		/// @note opcional.
		UIndex32 rootIndex = std::numeric_limits<UIndex32>::max();

		/// @brief Matrices inversas de la transformación global de los huesos en el estado inicial.
		DynamicArray<glm::mat4> inverseMatrices;

		/// @brief Huesos a los que se aplica la Skin. 
		/// También funciona como un mapa boneID -> nodeID.
		DynamicArray<UIndex32> bonesIds;

	};

}
