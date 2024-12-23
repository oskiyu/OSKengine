#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Vector3.hpp"
#include "Quaternion.h"

#include "Serializer.h"
#include "MutexHolder.h"
#include "AtomicHolder.h"

#include "Transform3D.h"
#include "DynamicArray.hpp"

namespace OSK::ECS {

	/// @brief Componente que contiene un transform 3D.
	/// Permite construir una jerarquía de transforms.
	class OSKAPI_CALL TransformComponent3D {

	public:

		OSK_SERIALIZABLE_COMPONENT();

	public:

		OSK_COMPONENT("OSK::TransformComponent3D");


		/// @param owner Objeto del transform.
		explicit TransformComponent3D(ECS::GameObjectIndex owner);

		/// @bug No funciona. 
		static TransformComponent3D FromMatrix(ECS::GameObjectIndex owner, const glm::mat4& matrix);


		/// @brief Añade un hijo al transform.
		/// @param childId ID del hijo.
		/// @pre @p childId debe poseer un TransformComponent3D.
		/// @pre @p childId no debe haber sido previamente añadido.
		void AddChild(ECS::GameObjectIndex childId);

		/// @brief Elimina un hijo de la lista de hijos.
		/// @param childId ID del hijo.
		void RemoveChild(ECS::GameObjectIndex childId);

		/// @return Lista con los ID de los elementos hijos.
		std::span<const GameObjectIndex> GetChildren() const;


		Transform3D& GetTransform();
		const Transform3D& GetTransform() const;


		/// @brief Permite establecer si el transform hereda la posición del padre.
		void SetShouldInheritPosition(bool value);

		/// @brief Permite establecer si el transform hereda la orientación del padre.
		void SetShouldInheritRotation(bool value);

		/// @brief Permite establecer si el transform hereda la escala del padre.
		void SetShouldInheritScale(bool value);


		/// @brief Aplica los cambios realizados sobre el transform.
		/// @param parent Transform padre (opcional).
		void _ApplyChanges(std::optional<const TransformComponent3D*> parent);

	private:

		Transform3D m_transform{};
		Transform3D::InheritanceConfig m_config{};

		/// @brief Identificador del objeto que este transform representa.
		GameObjectIndex m_ownerId = EMPTY_GAME_OBJECT;

		/// @brief IDs de los objetos hijos.
		DynamicArray<GameObjectIndex> m_childIds;

	};

}

OSK_COMPONENT_SERIALIZATION(OSK::ECS::TransformComponent3D);
