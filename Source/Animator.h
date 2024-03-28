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
	/// @todo Interpolaci�n de animaciones.
	class OSKAPI_CALL Animator {

	public:

		/// @brief Establece la matriz inicial de la animaci�n.
		/// @param initialTransform Matriz inicial.
		void Setup(const glm::mat4& initialTransform);


		/// @brief Actualiza el esqueleto del modelo, de acuerdo a todas
		/// las animaciones activas.
		/// @param deltaTime Tiempo que ha pasado desde la �ltima llamada
		/// a la funci�n.
		void Update(TDeltaTime deltaTime);


		/// @brief Establece la animaci�n activa del modelo.
		/// @param name Nombre de la animaci�n.
		/// 
		/// @pre Debe haber una animaci�n disponible con el nombre dado.
		/// @throws ModelAnimationNotFoundException si no se cumple la precondici�n.
		void AddActiveAnimation(std::string_view name);

		/// @brief Quita una animaci�n activa del modelo.
		/// @param name Nombre de la animaci�n.
		/// 
		/// @note Si no hay una animaci�n activa, no ocurrir� nada.
		/// 
		/// @pre Debe haber una animaci�n disponible con el nombre dado.
		/// @throws ModelAnimationNotFoundException si no se cumple la precondici�n.
		void RemoveActiveAnimation(std::string_view name);


		/// @brief Establece la skin activa del modelo,
		/// estableciendo qu� v�rtices ser�n afectados por la animaci�n.
		/// @param name Nombre de la skin.
		/// 
		/// @pre Debe haber una skin disponible con el nombre dado.
		/// @throws std::runtime_error si no se cumple la precondici�n.
		void SetActiveSkin(std::string_view name);

		/// @return Devuelve la skin activa. Puntero nulo si no hay ninguna skin activa.
		const AnimationSkin* GetActiveSkin() const;


		/// @param index �ndice de la skin.
		/// @return Devuelve la skin con el �ndice dado.
		/// 
		/// @pre Debe existir la skin con el �ndice dado.
		/// @throws ModelSkinNotFoundException si no se cumple la precondici�n.
		const AnimationSkin& GetSkin(UIndex32 index) const;
		

		/// @return Matriz inicial de la animaci�n.
		glm::mat4 GetInitialTransform() const;


		/// @brief Registra una nueva animaci�n.
		/// @param animation Nueva animaci�n.
		void AddAnimation(const Animation& animation);

		/// @brief Registra una nueva skin.
		/// @param skin Nueva skin.
		void AddSkin(const AnimationSkin& skin);


		/// @return Lista con las matrices de los huesos del esqueleto
		/// del modelo, con todas las animaciones activas aplicadas.
		const DynamicArray<glm::mat4>& GetFinalSkeletonMatrices() const;


		/// @return True si contiene al menos una animaci�n.
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
