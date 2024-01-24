#pragma once

#include "DynamicArray.hpp"
#include "HashMap.hpp"

#include "Bone.h"

namespace OSK::GRAPHICS {

	struct AnimationSkin;

	class OSKAPI_CALL Skeleton {

	public:

		Skeleton() = default;

		OSK_DEFAULT_COPY_OPERATOR(Skeleton);
		OSK_DEFAULT_MOVE_OPERATOR(Skeleton);

		void UpdateMatrices(const AnimationSkin& skin);

		/// <summary> Devuelve el nodo con el �ndice dado. </summary>
		/// <param name="nodeIndex">�ndice del nodo.</param>
		/// 
		/// @pre Debe existir un nodo con el �ndice dado.
		/// @throws BoneNotFoundException si no existe un hueso con el �ndice dado.
		MeshNode& GetNode(UIndex32 nodeIndex);
		const MeshNode& GetNode(UIndex32 nodeIndex) const {
			return GetNode(nodeIndex);
		}

		/// <summary> Devuelve el hueso con el �ndice dado. </summary>
		/// <param name="boneIndex">�ndice del hueso.</param>
		/// <returns>Puntero nulo si no hay una skin activa.</returns>
		/// 
		/// @pre Debe existir un hueso con el �ndice dado en la animaci�n activa.
		/// @throws BoneNotFoundException si no existe un hueso con el �ndice dado.
		Bone& GetBone(UIndex32 boneIndex, const AnimationSkin& skin);

		/// <summary> Devuelve el hueso con el �ndice dado. </summary>
		/// <param name="boneIndex">�ndice del hueso.</param>
		/// <returns>Puntero nulo si no hay una skin activa.</returns>
		/// 
		/// @pre Debe existir un hueso con el �ndice dado en la animaci�n activa.
		/// @throws BoneNotFoundException si no existe un hueso con el �ndice dado.
		const Bone& GetBone(UIndex32 boneIndex, const AnimationSkin& skin) const;

		/// <summary> Devuelve el nodo con el nombre dado. </summary>
		/// <param name="name">Nombre del nodo.</param>
		/// <returns>Puntero no nulo.</returns>
		/// 
		/// @pre Debe existir un nodo con el nombre dado.
		/// @throws BoneNotFoundException si no existe un hueso con el nombre dado.
		MeshNode& GetNode(std::string_view name);
		const MeshNode& GetNode(std::string_view name) const {
			return GetNode(name);
		}

		/// <summary> Devuelve el hueso con el nombre dado. </summary>
		/// <param name="name">Nombre del hueso.</param>
		/// 
		/// @pre Debe existir un hueso con el �ndice dado en la animaci�n activa.
		/// @throws BoneNotFoundException si no existe un hueso con el nombre dado.
		Bone& GetBone(std::string_view name, const AnimationSkin& skin);
		const Bone& GetBone(std::string_view name, const AnimationSkin& skin) const {
			return GetBone(name, skin);
		}

		/// @brief A�ade un nodo al esqueleto.
		/// @param node Nuevo nodo.
		/// @throws BoneAlreadyAddedException Si el bone ya estaba antes a�adido.
		void _AddNode(const MeshNode& node);

		/// <summary> Devuelve el �ndice nodo del esqueleto que no tiene padre. </summary>
		UIndex32 GetRootNodeIndex() const;

	private:

		/// <summary> Mapa �ndice del nodo -> nodo. </summary>
		/// @note No todos los nodos son huesos.
		std::unordered_map<UIndex32, MeshNode> nodes;

		/// <summary> Mapa nombre del nodo -> �ndice del nodo. </summary>
		std::unordered_map<std::string, UIndex32, StringHasher, std::equal_to<>> nodesByName;

	};

}
