#pragma once

#include "DynamicArray.hpp"
#include "Vector3.hpp"
#include "Quaternion.h"

#include <limits>

#ifdef max
#undef max
#endif

namespace OSK::GRAPHICS {

	class Skeleton;
	struct AnimationSkin;

	/// <summary> Hueso de la animación. </summary>
	using Bone = struct MeshNode {

		/// <summary> Actualiza la matriz propia y la de los huesos hijos. </summary>
		/// <param name="prevMatrix">Matriz del hueso padre.</param>
		/// <param name="animator">Animator que contiene los huesos hijos.</param>
		void UpdateSkeletonTree(const glm::mat4& prevMatrix, Skeleton* skeleton);

		/// <summary> Nombre nativo. </summary>
		std::string name;

		/// <summary> Posición actual. </summary>
		Vector3f position;
		/// <summary> Orientación actual. </summary>
		Quaternion rotation;
		/// <summary> Escala actual. </summary>
		Vector3f scale = Vector3f(1.0f);

		/// <summary> Índice del hueso. </summary>
		UIndex32 thisIndex = 0;
		/// <summary> Índice del nodo padre. </summary>
		UIndex32 parentIndex = std::numeric_limits<UIndex32>::max();
		/// <summary> Índices de los nodos hijos. </summary>
		DynamicArray<UIndex32> childIndices;

		UIndex32 skinIndex = std::numeric_limits<UIndex32>::max();

		/// <summary> Devuelve la matriz de modelo del hueso. </summary>
		inline glm::mat4 GetLocalMatrix() const {
			return glm::translate(glm::mat4(1.0f), position.ToGlm()) * glm::mat4(rotation.ToGlm()) * glm::scale(glm::mat4(1.0f), scale.ToGlm());
		}

		glm::mat4 globalMatrix = glm::mat4(1.0f);

	};

}
