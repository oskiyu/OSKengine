#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Vector3.hpp"
#include "Quaternion.h"

#include "Serializer.h"
#include "MutexHolder.h"
#include "AtomicHolder.h"


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

				
		/// @brief Establece la posici�n local.
		/// @param position Nueva posici�n respecto al padre.
		void SetPosition(const Vector3f& position);

		/// @brief Establece la escala local.
		/// @param scale Nueva escala respecto al padre.
		void SetScale(const Vector3f& scale);

		/// @brief Establece la rotaci�n local.
		/// @param rotation Nueva rotaci�n respecto al padre.
		void SetRotation(const Quaternion& rotation);


		/// @brief Suma el vector 3D a la posici�n.
		/// @param positionDelta Cambio de posici�n.
		void AddPosition(const Vector3f& positionDelta);

		/// @brief Suma el vector 3D a la posici�n.
		/// @param positionDelta Cambio de posici�n.
		/// 
		/// @pre No debe haber ning�n otro hilo llamando a _ApplyChanges().
		/// @pre No debe haber ning�n otro hilo lllamando a AddPosition().
		/// @threadsafety Esta funci�n es thread-safe, siempre y
		/// cuando se cumplan las precondiciones.
		/// 
		/// @note En condiciones normales, s�lamente se llama a _ApplyChanges()
		/// en el sistema TransformApplierSystem (que se ejecuta de
		/// manera exclusiva).
		void AddPosition_ThreadSafe(const Vector3f& positionDelta);


		/// @brief Suma un vector 3D a la escala.
		/// @param scaleDelta Cambio de escala.
		void AddScale(const Vector3f& scaleDelta);

		/// @brief Suma un vector 3D a la escala.
		/// @param scaleDelta Cambio de escala.
		/// 
		/// @pre No debe haber ning�n otro hilo llamando a _ApplyChanges().
		/// @pre No debe haber ning�n otro hilo lllamando a AddPosition().
		/// @threadsafety Esta funci�n es thread-safe, siempre y
		/// cuando se cumplan las precondiciones.
		/// 
		/// @note En condiciones normales, s�lamente se llama a _ApplyChanges()
		/// en el sistema TransformApplierSystem (que se ejecuta de
		/// manera exclusiva).
		void AddScale_ThreadSafe(const Vector3f& scaleDelta);


		/// @brief Aplica una rotaci�n al transform.
		/// @param rotationDelta Cambio de rotaci�n.
		void ApplyRotation(const Quaternion& rotationDelta);

		/// @brief Aplica una rotaci�n al transform.
		/// @param rotationDelta Cambio de rotaci�n.
		/// 
		/// @pre No debe haber ning�n otro hilo llamando a _ApplyChanges().
		/// @pre No debe haber ning�n otro hilo lllamando a AddPosition().
		/// @threadsafety Esta funci�n es thread-safe, siempre y
		/// cuando se cumplan las precondiciones.
		/// 
		/// @note En condiciones normales, s�lamente se llama a _ApplyChanges()
		/// en el sistema TransformApplierSystem (que se ejecuta de
		/// manera exclusiva).
		void ApplyRotation_ThreadSafe(const Quaternion& rotationDelta);

		
		/// @brief Rota el transform respecto a s� mismo.
		/// @param angle �ngulo.
		/// @param axis Eje sobre el que se rota.
		/// @pre El �ngulo debe estar en grados.
		void RotateLocalSpace(float angle, const Vector3f& axis);

		/// @brief Rota el transform respecto a s� mismo.
		/// @param angle �ngulo.
		/// @param axis Eje sobre el que se rota.
		/// @pre El �ngulo debe estar en grados.
		/// 
		/// @pre No debe haber ning�n otro hilo llamando a _ApplyChanges().
		/// @pre No debe haber ning�n otro hilo lllamando a AddPosition().
		/// @threadsafety Esta funci�n es thread-safe, siempre y
		/// cuando se cumplan las precondiciones.
		/// 
		/// @note En condiciones normales, s�lamente se llama a _ApplyChanges()
		/// en el sistema TransformApplierSystem (que se ejecuta de
		/// manera exclusiva).
		void RotateLocalSpace_ThreadSafe(float angle, const Vector3f& axis);


		/// @brief Rota el transform respecto al mundo.
		/// @param angle �ngulo.
		/// @param axis Eje sobre el que se rota.
		/// @pre El �ngulo debe estar en grados.
		void RotateWorldSpace(float angle, const Vector3f& axis);

		/// @brief Rota el transform respecto al mundo.
		/// @param angle �ngulo.
		/// @param axis Eje sobre el que se rota.
		/// @pre El �ngulo debe estar en grados.
		/// 
		/// @pre No debe haber ning�n otro hilo llamando a _ApplyChanges().
		/// @pre No debe haber ning�n otro hilo lllamando a AddPosition().
		/// @threadsafety Esta funci�n es thread-safe, siempre y
		/// cuando se cumplan las precondiciones.
		/// 
		/// @note En condiciones normales, s�lamente se llama a _ApplyChanges()
		/// en el sistema TransformApplierSystem (que se ejecuta de
		/// manera exclusiva).
		void RotateWorldSpace_ThreadSafe(float angle, const Vector3f& axis);


		/// @brief Transforma un punto respecto a este transform.
		/// @param point Punto antes de la transformaci�n.
		/// @return Punto transformado.
		Vector3f TransformPoint(const Vector3f& point) const;


		/// @brief A�ade un hijo al transform.
		/// @param childId ID del hijo.
		/// @pre @p childId debe poseer un Transform3D.
		/// @pre @p childId no debe haber sido previamente a�adido.
		void AddChild(ECS::GameObjectIndex childId);

		/// @brief Elimina un hijo de la lista de hijos.
		/// @param childId ID del hijo.
		void RemoveChild(ECS::GameObjectIndex childId);

		/// @return Lista con los ID de los elementos hijos.
		std::span<const GameObjectIndex> GetChildren() const;


		/// @return Posici�n en el mundo 3D.
		Vector3f GetPosition() const;

		/// @return Escala en el mundo 3D.
		Vector3f GetScale() const;
		

		/// @return Posici�n local (respecto al padre).
		Vector3f GetLocalPosition() const;

		/// @return Escala local (respecto al padre).
		Vector3f GetLocalScale() const;


		/// @return Orientaci�n local (respecto al padre).
		Quaternion GetLocalRotation() const;

		/// @return Orientaci�n en el mundo 3D.
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


		/// @brief Permite establecer si el transform hereda la posici�n del padre.
		void SetShouldInheritPosition(bool value);

		/// @brief Permite establecer si el transform hereda la orientaci�n del padre.
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

		/// @brief Vector posici�n en el mundo 3D.
		Vector3f m_globalPosition = Vector3f::Zero;

		/// @brief Vector escala en el mundo 3D.
		Vector3f m_globalScale = Vector3f::One;


		/// @brief Posici�n respecto al padre.
		Vector3f m_localPosition = Vector3f::Zero;

		/// @brief Escala respecto al padre.
		Vector3f m_localScale = Vector3f::One;


		/// @brief Orientaci�n respecto al padre.
		Quaternion m_localRotation{};

		/// @brief Rotaci�n global.
		glm::mat4 m_globalRotation = glm::mat4(1.0f);


		/// @brief Identificador del objeto que este transform representa.
		GameObjectIndex m_ownerId = EMPTY_GAME_OBJECT;

		/// @brief IDs de los objetos hijos.
		DynamicArray<GameObjectIndex> m_childIds;


		// --- Changes --- //

		AtomicHolder<bool> m_isPositionDirty = AtomicHolder<bool>(false);
		AtomicHolder<bool> m_isRotationDirty = AtomicHolder<bool>(false);
		AtomicHolder<bool> m_isScaleDirty = AtomicHolder<bool>(false);

		Vector3f m_changeInPosition = Vector3f::Zero;
		Quaternion m_changeInRotation = {};
		Vector3f m_changeInScale = Vector3f::Zero;


		// Multithreading

		MutexHolder m_positionMutex;
		MutexHolder m_rotationMutex;
		MutexHolder m_scaleMutex;

	};

}

namespace OSK::PERSISTENCE {

	template <>
	nlohmann::json SerializeJson<OSK::ECS::Transform3D>(const OSK::ECS::Transform3D& data);

	template <>
	OSK::ECS::Transform3D DeserializeJson<OSK::ECS::Transform3D>(const nlohmann::json& json);

}
