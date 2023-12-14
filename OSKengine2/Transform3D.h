#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Vector3.hpp"
#include "Quaternion.h"

namespace OSK::ECS {

	class OSKAPI_CALL Transform3D {

	public:

		OSK_COMPONENT("OSK::Transform3D");


		/// @param owner Objeto del transform.
		explicit Transform3D(ECS::GameObjectIndex owner);

				
		/// @brief Establece la posici�n local.
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param position Nueva posici�n respecto al padre.
		void SetPosition(const Vector3f& position);

		/// @brief Establece la escala local.
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param scale Nueva escala respecto al padre.
		void SetScale(const Vector3f& scale);

		/// @brief Establece la rotaci�n local.
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param rotation Nueva rotaci�n respecto al padre.
		void SetRotation(const Quaternion& rotation);


		/// @brief Suma el vector 3D a la posici�n.
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param positionDelta Cambio de posici�n.
		void AddPosition(const Vector3f& positionDelta);

		/// @brief Suma un vector 3D a la escala.
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param scaleDelta Cambio de escala.
		void AddScale(const Vector3f& scaleDelta);

		/// @brief Aplica una rotaci�n al transform.
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// 
		/// @param rotationDelta Cambio de rotaci�n.
		void ApplyRotation(const Quaternion& rotationDelta);

		
		/// @brief Rota el transform respecto a s� mismo.
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// @param angle �ngulo.
		/// @param axis Eje sobre el que se rota.
		/// @pre El �ngulo debe estar en grados.
		void RotateLocalSpace(float angle, const Vector3f& axis);

		/// @brief Rota el transform respecto al mundo.
		/// @param angle �ngulo.
		/// @param axis Eje sobre el que se rota.
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// @pre El �ngulo debe estar en grados.
		void RotateWorldSpace(float angle, const Vector3f& axis);


		/// @brief Actualiza la matriz modelo, y la de sus hijos.
		void UpdateModel();


		/// @brief Transforma un punto respecto a este transform.
		/// @param point Punto antes de la transformaci�n.
		/// @return Punto transformado.
		Vector3f TransformPoint(const Vector3f& point) const;


		/// @brief Enlaza este transform a su nuevo transform padre.
		/// @param baseTransform Tranform padre.
		void AttachToObject(ECS::GameObjectIndex baseTransform);

		/// @brief Libera este transform de su padre.
		/// Si no tiene padre, no ocurre nada.
		void UnAttach();


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


		/// @return Identificador del objeto que posee al
		/// transform padre.
		ECS::GameObjectIndex GetParentObject() const;


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

	private:

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
		GameObjectIndex owner = EMPTY_GAME_OBJECT;

		/// @brief Objeto padre (0 si no tiene).
		GameObjectIndex parent = EMPTY_GAME_OBJECT;

		/// @brief IDs de los objetos hijos.
		DynamicArray<GameObjectIndex> childTransforms;

	};

}
