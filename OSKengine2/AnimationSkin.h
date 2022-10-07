#pragma once

#include "DynamicArray.hpp"
#include "MaterialInstance.h"
#include "IGpuStorageBuffer.h"

#include <string>
#include <glm.hpp>

namespace OSK::GRAPHICS {

	class Animator;

	/// <summary> Indica c�mo aplicar una animaci�n a un modelo animado. </summary>
	struct AnimationSkin {

		/// <summary> Nombre original de la animaci�n. </summary>
		std::string name;

		/// <summary> Matrices inversas de la transformaci�n global de los huesos en el estado inicial. </summary>
		DynamicArray<glm::mat4> inverseMatrices;

		/// <summary> 
		/// Huesos a los que se aplica la Skin. 
		/// Tambi�n funciona como un mapa boneID -> nodeID.
		/// </summary>
		DynamicArray<TIndex> bonesIds;

	};

}
