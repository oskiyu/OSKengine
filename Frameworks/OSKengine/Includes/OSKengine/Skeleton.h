#pragma once

#include "DynamicArray.hpp"
#include "HashMap.hpp"

#include "Bone.h"

namespace OSK::GRAPHICS {

	class AnimationSkin;

	class OSKAPI_CALL Skeleton {

	public:

		void UpdateMatrices(const AnimationSkin& skin);

		/// <summary> Devuelve el nodo con el índice dado. </summary>
		/// <param name="nodeIndex">Índice del nodo.</param>
		/// <returns>Puntero no nulo.</returns>
		/// 
		/// @pre Debe existir un nodo con el índice dado.
		/// @throws std::runtime_error si se incumple la precondición.
		MeshNode* GetNode(TIndex nodeIndex) const;

		/// <summary> Devuelve el hueso con el índice dado. </summary>
		/// <param name="boneIndex">Índice del hueso.</param>
		/// <returns>Puntero nulo si no hay una skin activa.</returns>
		/// 
		/// @pre Debe existir un hueso con el índice dado en la animación activa.
		/// @throws std::runtime_error si se incumple la precondición.
		Bone* GetBone(TIndex boneIndex, const AnimationSkin& skin) const;

		/// <summary> Devuelve el nodo con el nombre dado. </summary>
		/// <param name="name">Nombre del nodo.</param>
		/// <returns>Puntero no nulo.</returns>
		/// 
		/// @pre Debe existir un nodo con el nombre dado.
		/// @throws std::runtime_error si se incumple la precondición.
		MeshNode* GetNode(const std::string& name) const;

		/// <summary> Devuelve el hueso con el nombre dado. </summary>
		/// <param name="name">Nombre del hueso.</param>
		/// 
		/// @pre Debe existir un hueso con el índice dado en la animación activa.
		/// @throws std::runtime_error si se incumple la precondición.
		Bone* GetBone(const std::string& name, const AnimationSkin& skin) const;

		/// <summary> Añade un nodo al esqueleto. </summary>
		void _AddNode(const MeshNode& node);

		/// <summary> Devuelve el índice nodo del esqueleto que no tiene padre. </summary>
		TIndex GetRootNodeIndex() const;

	private:

		/// <summary> Mapa índice del nodo -> nodo. </summary>
		/// @note No todos los nodos son huesos.
		HashMap<TIndex, MeshNode> nodes;

		/// <summary> Mapa nombre del nodo -> índice del nodo. </summary>
		HashMap<std::string, TIndex> nodesByName;

	};

}
