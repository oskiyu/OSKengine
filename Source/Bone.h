#pragma once

#include "DynamicArray.hpp"
#include "Vector3.hpp"
#include "Quaternion.h"

#include <limits>

#ifdef max
#undef max
#endif
#include <optional>

namespace OSK::GRAPHICS {

	class Skeleton;
	struct AnimationSkin;


	/// @brief Hueso de la animación.
	struct AnimationBone {

		/// @brief Actualiza la matriz propia y la de los huesos hijos.
		/// @param prevMatrix Matriz del hueso padre.
		/// @param skeleton Animator que contiene los huesos hijos.
		void UpdateSkeletonTree(const glm::mat4& prevMatrix, Skeleton* skeleton);


		/// @brief Nombre nativo.
		std::string name;

		/// @brief Posición actual.
		Vector3f position = Vector3f::Zero;
		/// @brief Orientación actual.
		Quaternion rotation{};
		/// @brief Escala actual.
		Vector3f scale = Vector3f::One;


		/// @brief Índice del hueso.
		UIndex32 thisIndex = 0;
		/// @brief Índice del nodo padre.
		std::optional<UIndex32> parentIndex;
		/// @brief Índices de los nodos hijos.
		DynamicArray<UIndex32> childIndices;

		std::optional<UIndex32> skinIndex;

		/// @return Matriz de modelo del hueso.
		inline glm::mat4 GetLocalMatrix() const {
			return
				glm::translate(glm::mat4(1.0f), position.ToGlm()) *
				glm::mat4(rotation.ToGlm()) * 
				glm::scale(glm::mat4(1.0f), scale.ToGlm());
		}

		glm::mat4 globalMatrix = glm::mat4(1.0f);

	};

}
