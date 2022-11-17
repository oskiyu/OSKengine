#pragma once

#include "DynamicArray.hpp"
#include "MaterialInstance.h"
#include "IGpuStorageBuffer.h"

#include <string>
#include <glm.hpp>

#include <limits>

namespace OSK::GRAPHICS {

	class Animator;

	/// <summary> Indica cómo aplicar una animación a un modelo animado. </summary>
	struct AnimationSkin {

		/// <summary> Nombre original de la animación. </summary>
		std::string name;


		/// <summary> Índice de este nodo. </summary>
		TIndex thisIndex = std::numeric_limits<TIndex>::max();

		/// <summary>
		/// Índice del nodo  root de la skin.
		/// </summary>
		/// @note opcional.
		TIndex rootIndex = std::numeric_limits<TIndex>::max();

		/// <summary> Matrices inversas de la transformación global de los huesos en el estado inicial. </summary>
		DynamicArray<glm::mat4> inverseMatrices;

		/// <summary> 
		/// Huesos a los que se aplica la Skin. 
		/// También funciona como un mapa boneID -> nodeID.
		/// </summary>
		DynamicArray<TIndex> bonesIds;

	};

}
