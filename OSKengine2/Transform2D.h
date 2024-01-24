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
	/// 
	/// @todo Serialización.
	class OSKAPI_CALL Transform2D {

	public:

		OSK_COMPONENT("OSK::Transform2D");


		/// @brief Transform por defecto.
		/// @param owner ID de la entidad que posee este transform.
		explicit Transform2D(ECS::GameObjectIndex owner);

		/// @brief Crea el transform.
		/// @param owner ID de la entidad que posee este transform.
		/// @param position Posición en el mundo.
		/// @param scale Escala en el mundo.
		/// @param rotation Rotación en el mundo.
		Transform2D(ECS::GameObjectIndex owner, const Vector2f& position, const Vector2f& scale, float rotation);


		/// <summary>
		/// Establece la posición.
		/// 
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="position">Nueva posición respecto al padre.</param>
		void SetPosition(const Vector2f& position);

		/// <summary>
		/// Establece la escala.
		/// 
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scale">Nueva escala respecto al padre.</param>
		void SetScale(const Vector2f& scale);

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
		void AddPosition(const Vector2f& positionDelta);

		/// <summary>
		/// Suma un vector 3D a la escala.
		/// 
		/// @note También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scaleDelta">Escala a añadir.</param>
		void AddScale(const Vector2f& scaleDelta);

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
		Vector2f GetPosition() const;

		/// <summary>
		/// Vector escala en el mundo 2D.
		/// </summary>
		Vector2f GetScale() const;

		/// <summary>
		/// Rotación en el mundo 2D.
		/// </summary>
		float GetRotation() const;

		/// <summary>
		/// Posición local.
		/// </summary>
		Vector2f GetLocalPosition() const;

		/// <summary>
		/// Escala local.
		/// </summary>
		Vector2f GetLocalScale() const;

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
		ECS::GameObjectIndex GetParentObject() const;

		/// <summary>
		/// Posición respecto al padre.
		/// </summary>
		Vector2f GetPositionOffset() const;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector2f GetScaleOffset() const;

		/// <summary>
		/// Rotación respecto al padre.
		/// </summary>
		float GetRotationOffset() const;

	private:

		/// <summary>
		/// Vector posición en el mundo 2D.
		/// </summary>
		Vector2f globalPosition;

		/// <summary>
		/// Vector escala en el mundo 2D.
		/// </summary>
		Vector2f globalScale;

		/// <summary>
		/// Rotación en el mundo 2D.
		/// </summary>
		float globalRotation;

		/// <summary>
		/// Posición local.
		/// </summary>
		Vector2f localPosition;

		/// <summary>
		/// Escala local.
		/// </summary>
		Vector2f localScale;

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
		Vector2f positionOffset;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector2f scaleOffset;

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
