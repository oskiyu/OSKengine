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

		void Setup(const glm::mat4& initialTransform);

		/// <summary> Actualiza el esqueleto de acuerdo a la animaci�n activa. </summary>
		/// @note Si no hay ninguna animaci�na ctiva, no ocurrir� nada.
		void Update(TDeltaTime deltaTime);

		/// <summary> Desactiva la animaci�n, de tal manera que no haya una animaci�n activa. </summary>
		/// @note Si no hay una animaci�n activa, no ocurrir� nada.
		void DeactivateAnimation();

		/// <summary> Establece la animaci�n activa del modelo. </summary>
		/// <param name="name">Nombre de la animaci�n.</param>
		/// 
		/// @pre Debe haber una animaci�n disponible con el nombre dado.
		/// @throws std::runtime_error si no se cumple la precondici�n.
		void AddActiveAnimation(const std::string& name);

		/// <summary> Quita una animaci�n activa del modelo. </summary>
		/// <param name="name">Nombre de la animaci�n.</param>
		/// 
		/// @note Si no hay una animaci�n activa, no ocurrir� nada.
		void RemoveActiveAnimation(const std::string& name);

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

		/// <summary>
		/// Devuelve la skin con el �ndice dado.
		/// </summary>
		/// 
		/// @pre Debe existir la skin con el �ndice dado.
		const AnimationSkin& GetSkin(TIndex index) const;
		
		/// <summary>
		/// Devuelve un material instance que �nicamente tiene 
		/// el slot "animation", que contiene las matrices de los huesos.
		/// </summary>
		/// <returns>Not null.</returns>
		const MaterialInstance* GetMaterialInstance() const;

		glm::mat4 GetInitialTransform() const;

		void _AddAnimation(const Animation& animation);
		void _AddSkin(AnimationSkin&& skin);
		void _AddNode(const MeshNode& node);

	private:

		/// <summary> Esqueleto resultado de combinar todas las animaciones activas. </summary>
		DynamicArray<glm::mat4> boneMatrices;
		
		DynamicArray<MeshNode> nodes;
				
		/// <summary> Buffers que almacenan las matrices de los huesos que se env�an al vertex shader. </summary>
		UniquePtr<IGpuStorageBuffer> boneBuffers[NUM_RESOURCES_IN_FLIGHT]{};

		/// <summary> Contiene un �nico material slot: "animation". </summary>
		UniquePtr<MaterialInstance> materialInstance;

		DynamicArray<AnimationSkin> availableSkins;

		HashMap<std::string, TIndex> availableSkinsByName;
		HashMap<std::string, Animation> availableAnimations;

		DynamicArray<std::string> activeAnimations;
		std::string activeSkin = "";

		glm::mat4 initialTransform = glm::mat4(1.0f);

	};

}