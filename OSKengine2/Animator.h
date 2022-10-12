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

		void Setup(const glm::mat4& initialTransform);

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
		void AddActiveAnimation(const std::string& name);

		/// <summary> Quita una animación activa del modelo. </summary>
		/// <param name="name">Nombre de la animación.</param>
		/// 
		/// @note Si no hay una animación activa, no ocurrirá nada.
		void RemoveActiveAnimation(const std::string& name);

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

		/// <summary>
		/// Devuelve la skin con el índice dado.
		/// </summary>
		/// 
		/// @pre Debe existir la skin con el índice dado.
		const AnimationSkin& GetSkin(TIndex index) const;
		
		/// <summary>
		/// Devuelve un material instance que únicamente tiene 
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
				
		/// <summary> Buffers que almacenan las matrices de los huesos que se envían al vertex shader. </summary>
		UniquePtr<IGpuStorageBuffer> boneBuffers[NUM_RESOURCES_IN_FLIGHT]{};

		/// <summary> Contiene un único material slot: "animation". </summary>
		UniquePtr<MaterialInstance> materialInstance;

		DynamicArray<AnimationSkin> availableSkins;

		HashMap<std::string, TIndex> availableSkinsByName;
		HashMap<std::string, Animation> availableAnimations;

		DynamicArray<std::string> activeAnimations;
		std::string activeSkin = "";

		glm::mat4 initialTransform = glm::mat4(1.0f);

	};

}
