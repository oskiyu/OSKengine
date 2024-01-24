#pragma once

#include "Vector2.hpp"
#include "OSKmacros.h"
#include "GameObject.h"
#include "Component.h"
#include "Vector4.hpp"

namespace OSK::ECS {

	/// <summary>
	/// Clase que almacena el 'transform' de un objeto en un mundo 2D.
	/// El transform contiene posici�n, escala y rotaci�n.
	/// A este transform se pueden enlazar otros transform ahijados,
	/// de tal manera que al cambiar el transform padre,
	/// el resto de transforms tambi�n ven sus variables cambiadas.
	/// </summary>
	/// 
	/// @todo Serializaci�n.
	class OSKAPI_CALL Transform2D {

	public:

		OSK_COMPONENT("OSK::Transform2D");


		/// @brief Transform por defecto.
		/// @param owner ID de la entidad que posee este transform.
		explicit Transform2D(ECS::GameObjectIndex owner);

		/// @brief Crea el transform.
		/// @param owner ID de la entidad que posee este transform.
		/// @param position Posici�n en el mundo.
		/// @param scale Escala en el mundo.
		/// @param rotation Rotaci�n en el mundo.
		Transform2D(ECS::GameObjectIndex owner, const Vector2f& position, const Vector2f& scale, float rotation);


		/// <summary>
		/// Establece la posici�n.
		/// 
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="position">Nueva posici�n respecto al padre.</param>
		void SetPosition(const Vector2f& position);

		/// <summary>
		/// Establece la escala.
		/// 
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scale">Nueva escala respecto al padre.</param>
		void SetScale(const Vector2f& scale);

		/// <summary>
		/// Establece la rotaci�n del transform.
		/// 
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotation">�ngulo.</param>
		void SetRotation(float rotation);

		/// <summary>
		/// Suma el vector 3D a la posici�n.
		/// 
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="positionDelta">Posici�n a a�adir.</param>
		void AddPosition(const Vector2f& positionDelta);

		/// <summary>
		/// Suma un vector 3D a la escala.
		/// 
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scaleDelta">Escala a a�adir.</param>
		void AddScale(const Vector2f& scaleDelta);

		/// <summary>
		/// Cambia la rotaci�n del transform.
		/// 
		/// @note Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotationDelta">�ngulo a�adido.</param>
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
		/// Devuelve un rect�ngulo con la posici�n y el tama�o de este transform.
		/// </summary>
		/// <returns>Rect�ngulo del transform.</returns>
		Vector4 GetRectangle() const;

		/// <summary>
		/// Vector posici�n en el mundo 2D.
		/// </summary>
		Vector2f GetPosition() const;

		/// <summary>
		/// Vector escala en el mundo 2D.
		/// </summary>
		Vector2f GetScale() const;

		/// <summary>
		/// Rotaci�n en el mundo 2D.
		/// </summary>
		float GetRotation() const;

		/// <summary>
		/// Posici�n local.
		/// </summary>
		Vector2f GetLocalPosition() const;

		/// <summary>
		/// Escala local.
		/// </summary>
		Vector2f GetLocalScale() const;

		/// <summary>
		/// Rotaci�n local.
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
		/// Posici�n respecto al padre.
		/// </summary>
		Vector2f GetPositionOffset() const;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector2f GetScaleOffset() const;

		/// <summary>
		/// Rotaci�n respecto al padre.
		/// </summary>
		float GetRotationOffset() const;

	private:

		/// <summary>
		/// Vector posici�n en el mundo 2D.
		/// </summary>
		Vector2f globalPosition;

		/// <summary>
		/// Vector escala en el mundo 2D.
		/// </summary>
		Vector2f globalScale;

		/// <summary>
		/// Rotaci�n en el mundo 2D.
		/// </summary>
		float globalRotation;

		/// <summary>
		/// Posici�n local.
		/// </summary>
		Vector2f localPosition;

		/// <summary>
		/// Escala local.
		/// </summary>
		Vector2f localScale;

		/// <summary>
		/// Rotaci�n local.
		/// </summary>
		float localRotation;

		/// <summary>
		/// Matriz modelo.
		/// </summary>
		glm::mat4 modelMatrix;

		/// <summary>
		/// Posici�n respecto al padre.
		/// </summary>
		Vector2f positionOffset;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector2f scaleOffset;

		/// <summary>
		/// Rotaci�n respecto al padre.
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
