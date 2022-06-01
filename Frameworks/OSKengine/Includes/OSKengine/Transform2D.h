#pragma once

#include "Vector2.hpp"
#include "OSKmacros.h"
#include "GameObject.h"
#include "Component.h"
#include "Vector4.hpp"

namespace OSK::ECS {

	/// <summary>
	/// Clase que almacena el 'transform' de un objeto en un mundo 2D.
	/// El transform contiene posición, escala y rotación.
	/// A este transform se pueden enlazar otros transform ahijados,
	/// de tal manera que al cambiar el transform padre,
	/// el resto de transforms también ven sus variables cambiadas.
	/// </summary>
	class OSKAPI_CALL Transform2D {

	public:

		OSK_COMPONENT("OSK::Transform2D");

		/// <summary>
		/// Transform por defecto.
		/// </summary>
		Transform2D(ECS::GameObjectIndex owner);

		/// <summary>
		/// Crea el transform.
		/// </summary>
		/// <param name="position">Posición en el mundo.</param>
		/// <param name="scale">Escala en el mundo.</param>
		/// <param name="rotation">Rotación en el mundo.</param>
		Transform2D(ECS::GameObjectIndex owner, const Vector2& position, const Vector2& scale, float rotation);

		/// <summary>
		/// Establece la posición.
		/// 
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="position">Nueva posición respecto al padre.</param>
		void SetPosition(const Vector2& position);

		/// <summary>
		/// Establece la escala.
		/// 
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scale">Nueva escala respecto al padre.</param>
		void SetScale(const Vector2& scale);

		/// <summary>
		/// Establece la rotación del transform.
		/// 
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotation">Ángulo.</param>
		void SetRotation(float rotation);

		/// <summary>
		/// Suma el vector 3D a la posición.
		/// 
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="positionDelta">Posición a añadir.</param>
		void AddPosition(const Vector2& positionDelta);

		/// <summary>
		/// Suma un vector 3D a la escala.
		/// 
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scaleDelta">Escala a añadir.</param>
		void AddScale(const Vector2& scaleDelta);

		/// <summary>
		/// Cambia la rotación del transform.
		/// 
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotationDelta">Ángulo añadido.</param>
		void AddRotation(float rotationDelta);

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
		/// Devuelve un rectángulo con la posición y el tamaño de este transform.
		/// </summary>
		/// <returns>Rectángulo del transform.</returns>
		Vector4 GetRectangle() const;

		/// <summary>
		/// Vector posición en el mundo 2D.
		/// </summary>
		Vector2 GetPosition() const;

		/// <summary>
		/// Vector escala en el mundo 2D.
		/// </summary>
		Vector2 GetScale() const;

		/// <summary>
		/// Rotación en el mundo 2D.
		/// </summary>
		float GetRotation() const;

		/// <summary>
		/// Posición local.
		/// </summary>
		Vector2 GetLocalPosition() const;

		/// <summary>
		/// Escala local.
		/// </summary>
		Vector2 GetLocalScale() const;

		/// <summary>
		/// Rotación local.
		/// </summary>
		float GetLocalRotation() const;

		/// <summary>
		/// Matriz modelo.
		/// </summary>
		glm::mat4 GetAsMatrix() const;

		/// <summary>
		/// Transform padre.
		/// </summary>
		ECS::GameObjectIndex GetParentObject();

		/// <summary>
		/// Posición respecto al padre.
		/// </summary>
		Vector2 GetPositionOffset() const;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector2 GetScaleOffset() const;

		/// <summary>
		/// Rotación respecto al padre.
		/// </summary>
		float GetRotationOffset() const;

	private:

		/// <summary>
		/// Vector posición en el mundo 2D.
		/// </summary>
		Vector2 globalPosition;

		/// <summary>
		/// Vector escala en el mundo 2D.
		/// </summary>
		Vector2 globalScale;

		/// <summary>
		/// Rotación en el mundo 2D.
		/// </summary>
		float globalRotation;

		/// <summary>
		/// Posición local.
		/// </summary>
		Vector2 localPosition;

		/// <summary>
		/// Escala local.
		/// </summary>
		Vector2 localScale;

		/// <summary>
		/// Rotación local.
		/// </summary>
		float localRotation;

		/// <summary>
		/// Matriz modelo.
		/// </summary>
		glm::mat4 modelMatrix;

		/// <summary>
		/// Posición respecto al padre.
		/// </summary>
		Vector2 positionOffset;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector2 scaleOffset;

		/// <summary>
		/// Rotación respecto al padre.
		/// </summary>
		float rotationOffset;

		/// <summary>
		/// Identificador del objeto que este transform representa.
		/// </summary>
		ECS::GameObjectIndex owner;

		/// <summary>
		/// Objeto padre.
		/// </summary>
		ECS::GameObjectIndex parent = ECS::EMPTY_GAME_OBJECT;

		/// <summary>
		/// Ids de los objetos hijos.
		/// </summary>
		DynamicArray<ECS::GameObjectIndex> childTransforms;

	};

}
