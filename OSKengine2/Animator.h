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
	/// modelos animados contendrán animaciones.
	/// 
	/// Contiene el esqueleto del modelo animado.
	/// </summary>
	/// 
	/// @todo Interpolación de animaciones.
	class OSKAPI_CALL Animator {

	public:

		void Setup();

		/// <summary> Actualiza el esqueleto de acuerdo a la animación activa. </summary>
		/// @note Si no hay ninguna animacióna ctiva, no ocurrirá nada.
		void Update(TDeltaTime deltaTime, const Vector3f& globalScale);

		/// <summary> Desactiva la animación, de tal manera que no haya una animación activa. </summary>
		/// @note Si no hay una animación activa, no ocurrirá nada.
		void DeactivateAnimation();

		/// <summary> Establece la animación activa del modelo. </summary>
		/// <param name="name">Nombre de la animación.</param>
		/// 
		/// @pre Debe haber una animación disponible con el nombre dado.
		/// @throws std::runtime_error si no se cumple la precondición.
		void SetActiveAnimation(const std::string& name);

		/// <summary> 
		/// Establece la skin activa del modelo,
		/// estableciendo qué vértices serán afectados por la animación.
		/// </summary>
		/// <param name="name">Nombre de la animación.</param>
		/// 
		/// @pre Debe haber una skin disponible con el nombre dado.
		/// @throws std::runtime_error si no se cumple la precondición.
		void SetActiveSkin(const std::string& name);

		/// <summary> Devuelve la skin activa. </summary>
		/// <returns>Puntero nulo si no hay ninguna skin activa.</returns>
		const AnimationSkin* GetActiveSkin() const;

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
		Bone* GetBone(TIndex boneIndex) const;

		/// <summary>
		/// Devuelve un material instance que únicamente tiene 
		/// el slot "animation", que contiene las matrices de los huesos.
		/// </summary>
		/// <returns>Not null.</returns>
		const MaterialInstance* GetMaterialInstance() const;

		void _AddNode(const MeshNode& node);
		void _AddAnimation(const Animation& animation);
		void _AddSkin(AnimationSkin&& skin);

	private:

		/// <summary> Mapa índice del nodo -> nodo. </summary>
		/// @note No todos los nodos son huesos.
		HashMap<TIndex, MeshNode> nodes;


		/// <summary> Matrices de los huesos que se envían al vertex shader. </summary>
		DynamicArray<glm::mat4> boneMatrices;
		/// <summary> Buffers que almacenan las matrices de los huesos que se envían al vertex shader. </summary>
		UniquePtr<IGpuStorageBuffer> boneBuffers[NUM_RESOURCES_IN_FLIGHT]{};

		/// <summary> Contiene un único material slot: "animation". </summary>
		UniquePtr<MaterialInstance> materialInstance;

		HashMap<std::string, AnimationSkin> availableSkins;
		HashMap<std::string, Animation> availableAnimations;

		std::string activeAnimation = "";
		std::string activeSkin = "";

	};

}
