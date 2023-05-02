#pragma once

#include "DynamicArray.hpp"

#include <string>
#include <glm/glm.hpp>

#include <limits>

namespace OSK::GRAPHICS {

	class Animator;

	/// @brief Indica c�mo aplicar una animaci�n a un modelo animado.
	struct AnimationSkin {

		/// @brief Nombre original de la animaci�n.
		std::string name;


		/// @brief �ndice de este nodo.
		TIndex thisIndex = std::numeric_limits<TIndex>::max();

		/// @brief �ndice del nodo  root de la skin.
		/// @note opcional.
		TIndex rootIndex = std::numeric_limits<TIndex>::max();

		/// @brief Matrices inversas de la transformaci�n global de los huesos en el estado inicial.
		DynamicArray<glm::mat4> inverseMatrices;

		/// @brief Huesos a los que se aplica la Skin. 
		/// Tambi�n funciona como un mapa boneID -> nodeID.
		DynamicArray<TIndex> bonesIds;

	};

}
