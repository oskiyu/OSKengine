#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vector>

namespace OSK {

	/// <summary>
	/// Clase que almacena el 'transform' de un objeto en un mundo 2D.
	/// El transform contiene posición, escala y rotación.
	/// A este transform se pueden enlazar otros transform ahijados,
	/// de tal manera que al cambiar el transform padre,
	/// el resto de transforms también ven sus variables cambiadas.
	/// </summary>
	class OSKAPI_CALL Transform2D {

	public:

		/// <summary>
		/// Transform por defecto.
		/// </summary>
		Transform2D();

		/// <summary>
		/// Crea el transform.
		/// </summary>
		/// <param name="position">Posición en el mundo.</param>
		/// <param name="scale">Escala en el mundo.</param>
		/// <param name="rotation">Rotación en el mundo.</param>
		Transform2D(const Vector2& position, const Vector2& scale, float rotation);

		/// <summary>
		/// Establece la posición.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="position">Nueva posición respecto al padre.</param>
		void SetPosition(const Vector2& position);

		/// <summary>
		/// Establece la escala.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scale">Nueva escala respecto al padre.</param>
		void SetScale(const Vector2& scale);

		/// <summary>
		/// Establece la rotación del transform.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotation">Ángulo.</param>
		void SetRotation(float rotation);

		/// <summary>
		/// Suma el vector 3D a la posición.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="positionDelta">Posición a añadir.</param>
		void AddPosition(const Vector2& positionDelta);

		/// <summary>
		/// Suma un vector 3D a la escala.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scaleDelta">Escala a añadir.</param>
		void AddScale(const Vector2& scaleDelta);

		/// <summary>
		/// Cambia la rotación del transform.
		/// También actualiza la matriz modelo, y la de sus hijos.
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
		void AttachTo(Transform2D* baseTransform);

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
		Vector2 GlobalPosition;

		/// <summary>
		/// Vector escala en el mundo 2D.
		/// </summary>
		Vector2 GlobalScale;

		/// <summary>
		/// Rotación en el mundo 2D.
		/// </summary>
		float GlobalRotation;

		/// <summary>
		/// Posición local.
		/// </summary>
		Vector2 Position;

		/// <summary>
		/// Escala local.
		/// </summary>
		Vector2 Scale;

		/// <summary>
		/// Rotación local.
		/// </summary>
		float Rotation;

		/// <summary>
		/// Matriz modelo.
		/// </summary>
		glm::mat4 ModelMatrix;

		/// <summary>
		/// Transform padre.
		/// </summary>
		Transform2D* ParentTransform;

		/// <summary>
		/// Transformaciones ahijadas.
		/// </summary>
		std::vector<Transform2D*> ChildTransforms;

		/// <summary>
		/// Posición respecto al padre.
		/// </summary>
		Vector2 PositionOffset;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector2 ScaleOffset;

		/// <summary>
		/// Rotación respecto al padre.
		/// </summary>
		float RotationOffset;

		/// <summary>
		/// Establece si ha de heredar la escala padre.
		/// </summary>
		bool UseParentScale = false;

	private:
		/// <summary>
		/// True si tiene padre.
		/// </summary>
		bool isAttached = false;

		/// <summary>
		/// True si tiene ahijados.
		/// </summary>
		bool isParent = false;

		/// <summary>
		/// Transform padre.
		/// </summary>
		Transform2D* parent = nullptr;

	};

}
