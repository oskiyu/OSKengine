#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Vector3.hpp"
#include "Quaternion.h"

namespace OSK::ECS {

	class OSKAPI_CALL Transform3D {

	public:

		OSK_COMPONENT("OSK::Transform3D");

		/// <param name="owner">Objeto del transform.</param>
		Transform3D(ECS::GameObjectIndex owner);

		/// <summary>
		/// Establece la posici�n.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="position">Nueva posici�n respecto al padre.</param>
		void SetPosition(const Vector3f& position);

		/// <summary>
		/// Establece la escala.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scale">Nueva escala respecto al padre.</param>
		void SetScale(const Vector3f& scale);

		/// <summary>
		/// Establece la rotaci�n.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotation">Nueva rotaci�n respecto al padre.</param>
		void SetRotation(const Quaternion& rotation);

		/// <summary>
		/// Suma el vector 3D a la posici�n.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="positionDelta">Posici�n a a�adir.</param>
		void AddPosition(const Vector3f& positionDelta);

		/// <summary>
		/// Suma un vector 3D a la escala.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scaleDelta">Escala a a�adir.</param>
		void AddScale(const Vector3f& scaleDelta);

		/// <summary>
		/// Aplica una rotaci�n al transform.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotationDelta">Rotaci�n a aplicar.</param>
		void ApplyRotation(const Quaternion& rotationDelta);

		/// <summary>
		/// Rota el transform respecto a s� mismo.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="angle">�ngulo.</param>
		/// <param name="axis">Eje sobre el que se rota.</param>
		void RotateLocalSpace(float angle, const Vector3f& axis);

		/// <summary>
		/// Rota el transform respecto al mundo.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="angle">�ngulo.</param>
		/// <param name="axis">Eje sobre el que se rota.</param>
		void RotateWorldSpace(float angle, const Vector3f& axis);

		/// <summary>
		/// Actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		void UpdateModel();

		/// <summary>
		/// Enlaza este transform a su nuevo transform padre.
		/// </summary>
		/// <param name="baseTransform">Tranform padre.</param>
		void AttachToObject(ECS::GameObjectIndex baseTransform);

		/// <summary>
		/// Libera este transform de su padre.
		/// </summary>
		void UnAttach();

		/// <summary>
		/// Vector posici�n en el mundo 3D.
		/// </summary>
		Vector3f GetPosition() const;

		/// <summary>
		/// Vector escala en el mundo 3D.
		/// </summary>
		Vector3f GetScale() const;

		/// <summary>
		/// Posici�n respecto al padre.
		/// </summary>
		Vector3f GetLocalPosition() const;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector3f GetLocalScale() const;

		/// <summary>
		/// Orientaci�n respecto al padre.
		/// </summary>
		Quaternion GetLocalRotation() const;

		/// <summary>
		/// Orientaci�n.
		/// </summary>
		Quaternion GetRotation() const;

		/// <summary>
		/// Matriz modelo.
		/// </summary>
		glm::mat4 GetAsMatrix() const;

		/// <summary>
		/// Devuelve el identificador del objeto que posee al
		/// transform padre..
		/// </summary>
		ECS::GameObjectIndex GetParentObject() const;

		/// <summary>
		/// Devuelve un vector unitario 3D que apunta hacia el frente de la entidad.
		/// </summary>
		Vector3f GetForwardVector() const;

		/// <summary>
		/// Devuelve un vector unitario 3D que apunta hacia la derecha de la entidad.
		/// </summary>
		Vector3f GetRightVector() const;

		/// <summary>
		/// Devuelve un vector unitario 3D que apunta hacia arriba de la entidad.
		/// </summary>
		Vector3f GetTopVector() const;

	private:

		/// <summary>
		/// Matriz modelo (para renderizado).
		/// </summary>
		glm::mat4 matrix;

		/// <summary>
		/// Vector posici�n en el mundo 3D.
		/// </summary>
		Vector3f globalPosition;

		/// <summary>
		/// Vector escala en el mundo 3D.
		/// </summary>
		Vector3f globalScale;

		/// <summary>
		/// Posici�n respecto al padre.
		/// </summary>
		Vector3f localPosition;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector3f localScale;

		/// <summary>
		/// Orientaci�n.
		/// </summary>
		Quaternion localRotation;

		/// <summary>
		/// GlobalRotation
		/// </summary>
		glm::mat4 globalRotation = glm::mat4(1.0f);

		/// <summary>
		/// Identificador del objeto que este transform representa.
		/// </summary>
		ECS::GameObjectIndex owner;

		/// <summary>
		/// Objeto padre (0 si no tiene).
		/// </summary>
		ECS::GameObjectIndex parent = 0;

		/// <summary>
		/// Ids de los objetos hijos.
		/// </summary>
		DynamicArray<ECS::GameObjectIndex> childTransforms;

	};

}
