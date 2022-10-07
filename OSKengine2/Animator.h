#pragma once

#include "Animation.h"
#include "AnimationSkin.h"

#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "IGpuStorageBuffer.h"

#include "HashMap.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Componente (no ECS) de un modelo animado.
	/// 
	/// Todos los modelos tienen uno, pero solo los
	/// modelos animados contendr�n animaciones.
	/// 
	/// Contiene el esqueleto del modelo animado.
	/// </summary>
	/// 
	/// @todo Interpolaci�n de animaciones.
	class OSKAPI_CALL Animator {

	public:

		void Setup();

		/// <summary> Actualiza el esqueleto de acuerdo a la animaci�n activa. </summary>
		/// @note Si no hay ninguna animaci�na ctiva, no ocurrir� nada.
		void Update(TDeltaTime deltaTime, const Vector3f& globalScale);

		/// <summary> Desactiva la animaci�n, de tal manera que no haya una animaci�n activa. </summary>
		/// @note Si no hay una animaci�n activa, no ocurrir� nada.
		void DeactivateAnimation();

		/// <summary> Establece la animaci�n activa del modelo. </summary>
		/// <param name="name">Nombre de la animaci�n.</param>
		/// 
		/// @pre Debe haber una animaci�n disponible con el nombre dado.
		/// @throws std::runtime_error si no se cumple la precondici�n.
		void SetActiveAnimation(const std::string& name);

		/// <summary> 
		/// Establece la skin activa del modelo,
		/// estableciendo qu� v�rtices ser�n afectados por la animaci�n.
		/// </summary>
		/// <param name="name">Nombre de la animaci�n.</param>
		/// 
		/// @pre Debe haber una skin disponible con el nombre dado.
		/// @throws std::runtime_error si no se cumple la precondici�n.
		void SetActiveSkin(const std::string& name);

		/// <summary> Devuelve la skin activa. </summary>
		/// <returns>Puntero nulo si no hay ninguna skin activa.</returns>
		const AnimationSkin* GetActiveSkin() const;

		/// <summary> Devuelve el nodo con el �ndice dado. </summary>
		/// <param name="nodeIndex">�ndice del nodo.</param>
		/// <returns>Puntero no nulo.</returns>
		/// 
		/// @pre Debe existir un nodo con el �ndice dado.
		/// @throws std::runtime_error si se incumple la precondici�n.
		MeshNode* GetNode(TIndex nodeIndex) const;

		/// <summary> Devuelve el hueso con el �ndice dado. </summary>
		/// <param name="boneIndex">�ndice del hueso.</param>
		/// <returns>Puntero nulo si no hay una skin activa.</returns>
		/// 
		/// @pre Debe existir un hueso con el �ndice dado en la animaci�n activa.
		/// @throws std::runtime_error si se incumple la precondici�n.
		Bone* GetBone(TIndex boneIndex) const;

		/// <summary>
		/// Devuelve un material instance que �nicamente tiene 
		/// el slot "animation", que contiene las matrices de los huesos.
		/// </summary>
		/// <returns>Not null.</returns>
		const MaterialInstance* GetMaterialInstance() const;

		void _AddNode(const MeshNode& node);
		void _AddAnimation(const Animation& animation);
		void _AddSkin(AnimationSkin&& skin);

	private:

		/// <summary> Mapa �ndice del nodo -> nodo. </summary>
		/// @note No todos los nodos son huesos.
		HashMap<TIndex, MeshNode> nodes;


		/// <summary> Matrices de los huesos que se env�an al vertex shader. </summary>
		DynamicArray<glm::mat4> boneMatrices;
		/// <summary> Buffers que almacenan las matrices de los huesos que se env�an al vertex shader. </summary>
		UniquePtr<IGpuStorageBuffer> boneBuffers[NUM_RESOURCES_IN_FLIGHT]{};

		/// <summary> Contiene un �nico material slot: "animation". </summary>
		UniquePtr<MaterialInstance> materialInstance;

		HashMap<std::string, AnimationSkin> availableSkins;
		HashMap<std::string, Animation> availableAnimations;

		std::string activeAnimation = "";
		std::string activeSkin = "";

	};

}
