#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Vector3.hpp"
#include "Quaternion.h"

#include "Serializer.h"


namespace OSK::ECS {

	class OSKAPI_CALL Transform3D {

	public:

		template <typename T>
		friend nlohmann::json PERSISTENCE::SerializeJson<T>(const T& data);

		template <typename T>
		friend T PERSISTENCE::DeserializeJson<T>(const nlohmann::json& data);

	public:

		OSK_COMPONENT("OSK::Transform3D");


		/// @param owner Objeto del transform.
		explicit Transform3D(ECS::GameObjectIndex owner);

		static Transform3D FromMatrix(ECS::GameObjectIndex owner, const glm::mat4& matrix);

				
		/// @brief Establece la posición local.
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param position Nueva posición respecto al padre.
		void SetPosition(const Vector3f& position);

		/// @brief Establece la escala local.
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param scale Nueva escala respecto al padre.
		void SetScale(const Vector3f& scale);

		/// @brief Establece la rotación local.
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param rotation Nueva rotación respecto al padre.
		void SetRotation(const Quaternion& rotation);


		/// @brief Suma el vector 3D a la posición.
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param positionDelta Cambio de posición.
		void AddPosition(const Vector3f& positionDelta);

		/// @brief Suma un vector 3D a la escala.
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param scaleDelta Cambio de escala.
		void AddScale(const Vector3f& scaleDelta);

		/// @brief Aplica una rotación al transform.
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param rotationDelta Cambio de rotación.
		void ApplyRotation(const Quaternion& rotationDelta);

		
		/// @brief Rota el transform respecto a sí mismo.
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// @param angle Ángulo.
		/// @param axis Eje sobre el que se rota.
		/// @pre El ángulo debe estar en grados.
		void RotateLocalSpace(float angle, const Vector3f& axis);

		/// @brief Rota el transform respecto al mundo.
		/// @param angle Ángulo.
		/// @param axis Eje sobre el que se rota.
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// @pre El ángulo debe estar en grados.
		void RotateWorldSpace(float angle, const Vector3f& axis);


		/// @brief Transforma un punto respecto a este transform.
		/// @param point Punto antes de la transformación.
		/// @return Punto transformado.
		Vector3f TransformPoint(const Vector3f& point) const;


		/// @brief Añade un hijo al transform.
		/// @param childId ID del hijo.
		/// @pre @p childId debe poseer un Transform3D.
		/// @pre @p childId no debe haber sido previamente añadido.
		void AddChild(ECS::GameObjectIndex childId);

		/// @brief Elimina un hijo de la lista de hijos.
		/// @param childId ID del hijo.
		void RemoveChild(ECS::GameObjectIndex childId);

		/// @return Lista con los ID de los elementos hijos.
		std::span<const GameObjectIndex> GetChildren() const;


		/// @return Posición en el mundo 3D.
		Vector3f GetPosition() const;

		/// @return Escala en el mundo 3D.
		Vector3f GetScale() const;
		

		/// @return Posición local (respecto al padre).
		Vector3f GetLocalPosition() const;

		/// @return Escala local (respecto al padre).
		Vector3f GetLocalScale() const;


		/// @return Orientación local (respecto al padre).
		Quaternion GetLocalRotation() const;

		/// @return Orientación en el mundo 3D.
		Quaternion GetRotation() const;


		/// @return Matriz modelo, para renderizado.
		glm::mat4 GetAsMatrix() const;

		/// @brief Actualiza manualmente el valor de la matriz modelo.
		/// @param matrix Nueva matriz modelo.
		void OverrideMatrix(const glm::mat4& matrix);


		/// @return Vector unitario 3D que apunta hacia el frente de la entidad.
		Vector3f GetForwardVector() const;

		/// @return Vector unitario 3D que apunta hacia la derecha de la entidad.
		Vector3f GetRightVector() const;

		/// @return Vector unitario 3D que apunta hacia arriba de la entidad.
		Vector3f GetTopVector() const;


		/// @brief Permite establecer si el transform hereda la posición del padre.
		void SetShouldInheritPosition(bool value);

		/// @brief Permite establecer si el transform hereda la orientación del padre.
		void SetShouldInheritRotation(bool value);

		/// @brief Permite establecer si el transform hereda la escala del padre.
		void SetShouldInheritScale(bool value);


		void _ApplyChanges(std::optional<const Transform3D*> parent);

	private:

		/// @brief Actualiza la matriz modelo.
		void UpdateModel(std::optional<const Transform3D*> parent);


		bool m_inheritPosition = true;
		bool m_inheritRotation = true;
		bool m_inheritScale = true;


		/// @brief Matriz modelo (para renderizado).
		glm::mat4 m_matrix = glm::mat4(1.0f);

		/// @brief Vector posición en el mundo 3D.
		Vector3f m_globalPosition = Vector3f::Zero;

		/// @brief Vector escala en el mundo 3D.
		Vector3f m_globalScale = Vector3f::One;


		/// @brief Posición respecto al padre.
		Vector3f m_localPosition = Vector3f::Zero;

		/// @brief Escala respecto al padre.
		Vector3f m_localScale = Vector3f::One;


		/// @brief Orientación respecto al padre.
		Quaternion m_localRotation{};

		/// @brief Rotación global.
		glm::mat4 m_globalRotation = glm::mat4(1.0f);


		/// @brief Identificador del objeto que este transform representa.
		GameObjectIndex m_ownerId = EMPTY_GAME_OBJECT;

		/// @brief IDs de los objetos hijos.
		DynamicArray<GameObjectIndex> m_childIds;


		// --- Changes --- //

		bool m_isPositionDirty = false;
		bool m_isRotationDirty = false;
		bool m_isScaleDirty = false;

		Vector3f m_changeInPosition = Vector3f::Zero;
		Quaternion m_changeInRotation = {};
		Vector3f m_changeInScale = Vector3f::Zero;

	};

}

namespace OSK::PERSISTENCE {

	template <>
	nlohmann::json SerializeJson<OSK::ECS::Transform3D>(const OSK::ECS::Transform3D& data);

	template <>
	OSK::ECS::Transform3D DeserializeJson<OSK::ECS::Transform3D>(const nlohmann::json& json);

}
