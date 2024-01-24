#pragma once

#include "Animation.h"
#include "AnimationSkin.h"

#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "GpuBuffer.h"

#include "HashMap.hpp"

#include <array>

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

		/// @brief Establece la animaci�n activa del modelo.
		/// @param name Nombre de la animaci�n.
		/// @pre Debe haber una animaci�n disponible con el nombre dado.
		/// @throws ModelAnimationNotFoundException si no se cumple la precondici�n.
		void AddActiveAnimation(std::string_view name);

		/// <summary> Quita una animaci�n activa del modelo. </summary>
		/// <param name="name">Nombre de la animaci�n.</param>
		/// 
		/// @note Si no hay una animaci�n activa, no ocurrir� nada.
		void RemoveActiveAnimation(std::string_view name);

		/// <summary> 
		/// Establece la skin activa del modelo,
		/// estableciendo qu� v�rtices ser�n afectados por la animaci�n.
		/// </summary>
		/// <param name="name">Nombre de la animaci�n.</param>
		/// 
		/// @pre Debe haber una skin disponible con el nombre dado.
		/// @throws std::runtime_error si no se cumple la precondici�n.
		void SetActiveSkin(std::string_view name);

		/// <summary> Devuelve la skin activa. </summary>
		/// <returns>Puntero nulo si no hay ninguna skin activa.</returns>
		const AnimationSkin* GetActiveSkin() const;

		/// <summary>
		/// Devuelve la skin con el �ndice dado.
		/// </summary>
		/// 
		/// @pre Debe existir la skin con el �ndice dado.
		const AnimationSkin& GetSkin(UIndex32 index) const;
		
		/// <summary>
		/// Devuelve un material instance que �nicamente tiene 
		/// el slot "animation", que contiene las matrices de los huesos.
		/// </summary>
		/// <returns>Not null.</returns>
		const MaterialInstance* GetMaterialInstance() const;

		glm::mat4 GetInitialTransform() const;

		void _AddAnimation(const Animation& animation);
		void _AddSkin(const AnimationSkin& skin);
		void _AddNode(const MeshNode& node);

	private:

		/// <summary> Esqueleto resultado de combinar todas las animaciones activas. </summary>
		DynamicArray<glm::mat4> m_boneMatrices;
		
		DynamicArray<MeshNode> m_nodes;
				
		/// <summary> Buffers que almacenan las matrices de los huesos que se env�an al vertex shader. </summary>
		std::array<UniquePtr<GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_boneBuffers{};

		/// <summary> Contiene un �nico material slot: "animation". </summary>
		UniquePtr<MaterialInstance> m_materialInstance;

		DynamicArray<AnimationSkin> m_availableSkins;

		std::unordered_map<std::string, UIndex64, StringHasher, std::equal_to<>>  m_availableSkinsByName;
		std::unordered_map<std::string, Animation, StringHasher, std::equal_to<>> m_availableAnimations;

		DynamicArray<std::string> m_activeAnimations;
		std::string m_activeSkin = "";

		glm::mat4 m_initialTransform = glm::mat4(1.0f);

	};

}
