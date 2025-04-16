#pragma once

#include "DynamicArray.hpp"
#include "HashMap.hpp"

#include "Bone.h"

namespace OSK::GRAPHICS {

	struct AnimationSkin;

	class OSKAPI_CALL Skeleton {

	public:

		explicit Skeleton(const DynamicArray<AnimationBone>& bones);

		OSK_DEFAULT_COPY_OPERATOR(Skeleton);
		OSK_DEFAULT_MOVE_OPERATOR(Skeleton);

		void UpdateMatrices(const AnimationSkin& skin);


		/// @brief A�ade un nuevo hueso al esqueleto.
		/// @param bone Nuevo hueso.
		/// 
		/// @pre No debe existir previamente un hueso con el mismo nombre.
		void AddBone(const AnimationBone& bone);

		/// @param boneIndex �ndice del nodo
		/// @return Nodo con el �ndice dado.
		/// 
		/// @pre Debe existir un nodo con el �ndice dado.
		/// @throws BoneNotFoundException si no existe un hueso con el �ndice dado.
		AnimationBone& GetBone(UIndex32 boneIndex);
		const AnimationBone& GetBone(UIndex32 boneIndex) const;

		/// @param boneIndex �ndice del hueso (en la skin).
		/// @param skin Skin con el identificador del hueso.
		/// @return Hueso con el �ndice dado (seg�n la skin indicada).
		/// 
		/// @pre Debe existir un hueso con el �ndice dado en la animaci�n activa.
		/// @throws BoneNotFoundException si no existe un hueso con el �ndice dado.
		AnimationBone& GetBone(UIndex32 boneIndex, const AnimationSkin& skin);
		const AnimationBone& GetBone(UIndex32 boneIndex, const AnimationSkin& skin) const;

		/// @param name Nombre del hueso.
		/// @return Hueso con el nombre dado.
		/// 
		/// @pre Debe existir un hueso con el nombre dado.
		/// @throws BoneNotFoundException si no existe un hueso con el nombre dado.
		AnimationBone& GetBone(std::string_view name);
		const AnimationBone& GetBone(std::string_view name) const;

		/// @param name Nombre del hueso.
		/// @param skin Skin.
		/// @return Hueso con el nombre dado, seg�n la skin indicada.
		/// 
		/// @pre Debe existir un hueso con el �ndice dado en la animaci�n activa.
		/// @throws BoneNotFoundException si no existe un hueso con el nombre dado.
		AnimationBone& GetBone(std::string_view name, const AnimationSkin& skin);
		const AnimationBone& GetBone(std::string_view name, const AnimationSkin& skin) const;


		/// @return �ndice del hueso del esqueleto que no tiene padre.
		UIndex32 GetRootNodeIndex() const;

		/// @return N�mero de huesos.
		USize64 GetBoneCount() const;

	private:

		/// @brief Mapa �ndice del nodo -> nodo.
		/// @note No todos los nodos son huesos.
		std::unordered_map<UIndex32, AnimationBone> m_bones;

		/// @brief Mapa nombre del nodo -> �ndice del nodo.
		std::unordered_map<std::string, UIndex32, StringHasher, std::equal_to<>> m_boneNameToIndex;
		
	};

}
