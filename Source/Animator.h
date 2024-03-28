#pragma once

#include "Animation.h"
#include "AnimationSkin.h"

#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "GpuBuffer.h"

#include "HashMap.hpp"

#include <array>
#include <unordered_set>


namespace OSK::GRAPHICS {

	/// @brief Contiene y gestiona las animaciones de un
	/// modelo 3D.
	/// 
	/// @todo Interpolación de animaciones.
	class OSKAPI_CALL Animator {

	public:

		/// @brief Establece la matriz inicial de la animación.
		/// @param initialTransform Matriz inicial.
		void Setup(const glm::mat4& initialTransform);


		/// @brief Actualiza el esqueleto del modelo, de acuerdo a todas
		/// las animaciones activas.
		/// @param deltaTime Tiempo que ha pasado desde la última llamada
		/// a la función.
		void Update(TDeltaTime deltaTime);


		/// @brief Establece la animación activa del modelo.
		/// @param name Nombre de la animación.
		/// 
		/// @pre Debe haber una animación disponible con el nombre dado.
		/// @throws ModelAnimationNotFoundException si no se cumple la precondición.
		void AddActiveAnimation(std::string_view name);

		/// @brief Quita una animación activa del modelo.
		/// @param name Nombre de la animación.
		/// 
		/// @note Si no hay una animación activa, no ocurrirá nada.
		/// 
		/// @pre Debe haber una animación disponible con el nombre dado.
		/// @throws ModelAnimationNotFoundException si no se cumple la precondición.
		void RemoveActiveAnimation(std::string_view name);


		/// @brief Establece la skin activa del modelo,
		/// estableciendo qué vértices serán afectados por la animación.
		/// @param name Nombre de la skin.
		/// 
		/// @pre Debe haber una skin disponible con el nombre dado.
		/// @throws std::runtime_error si no se cumple la precondición.
		void SetActiveSkin(std::string_view name);

		/// @return Devuelve la skin activa. Puntero nulo si no hay ninguna skin activa.
		const AnimationSkin* GetActiveSkin() const;


		/// @param index Índice de la skin.
		/// @return Devuelve la skin con el índice dado.
		/// 
		/// @pre Debe existir la skin con el índice dado.
		/// @throws ModelSkinNotFoundException si no se cumple la precondición.
		const AnimationSkin& GetSkin(UIndex32 index) const;
		

		/// @return Matriz inicial de la animación.
		glm::mat4 GetInitialTransform() const;


		/// @brief Registra una nueva animación.
		/// @param animation Nueva animación.
		void AddAnimation(const Animation& animation);

		/// @brief Registra una nueva skin.
		/// @param skin Nueva skin.
		void AddSkin(const AnimationSkin& skin);


		/// @return Lista con las matrices de los huesos del esqueleto
		/// del modelo, con todas las animaciones activas aplicadas.
		const DynamicArray<glm::mat4>& GetFinalSkeletonMatrices() const;


		/// @return True si contiene al menos una animación.
		/// Para modelos no animados, devuelve false.
		bool HasAnimations() const;

	private:

		/// @brief Esqueleto resultado de combinar todas las animaciones activas.
		DynamicArray<glm::mat4> m_boneMatrices;
		

		DynamicArray<Animation> m_animations;
		DynamicArray<AnimationSkin> m_animationSkins;

		std::unordered_set<UIndex64> m_activeAnimations;
		std::optional<UIndex64> m_activeSkinIndex;

		std::unordered_map<std::string, UIndex64, StringHasher, std::equal_to<>> m_skinsByName;
		std::unordered_map<std::string, UIndex64, StringHasher, std::equal_to<>> m_animationsByName;

		glm::mat4 m_initialTransform = glm::mat4(1.0f);

	};

}
