#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vector>

namespace OSK {

	/// <summary>
	/// Clase que almacena el 'transform' de un objeto en un mundo 2D.
	/// El transform contiene posici�n, escala y rotaci�n.
	/// A este transform se pueden enlazar otros transform ahijados,
	/// de tal manera que al cambiar el transform padre,
	/// el resto de transforms tambi�n ven sus variables cambiadas.
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
		/// <param name="position">Posici�n en el mundo.</param>
		/// <param name="scale">Escala en el mundo.</param>
		/// <param name="rotation">Rotaci�n en el mundo.</param>
		Transform2D(const Vector2& position, const Vector2& scale, float rotation);

		/// <summary>
		/// Establece la posici�n.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="position">Nueva posici�n respecto al padre.</param>
		void SetPosition(const Vector2& position);

		/// <summary>
		/// Establece la escala.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scale">Nueva escala respecto al padre.</param>
		void SetScale(const Vector2& scale);

		/// <summary>
		/// Establece la rotaci�n del transform.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotation">�ngulo.</param>
		void SetRotation(float rotation);

		/// <summary>
		/// Suma el vector 3D a la posici�n.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="positionDelta">Posici�n a a�adir.</param>
		void AddPosition(const Vector2& positionDelta);

		/// <summary>
		/// Suma un vector 3D a la escala.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scaleDelta">Escala a a�adir.</param>
		void AddScale(const Vector2& scaleDelta);

		/// <summary>
		/// Cambia la rotaci�n del transform.
		/// Tambi�n actualiza la matriz modelo, y la de sus hijos.
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
		void AttachTo(Transform2D* baseTransform);

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
		Vector2 GlobalPosition;

		/// <summary>
		/// Vector escala en el mundo 2D.
		/// </summary>
		Vector2 GlobalScale;

		/// <summary>
		/// Rotaci�n en el mundo 2D.
		/// </summary>
		float GlobalRotation;

		/// <summary>
		/// Posici�n local.
		/// </summary>
		Vector2 Position;

		/// <summary>
		/// Escala local.
		/// </summary>
		Vector2 Scale;

		/// <summary>
		/// Rotaci�n local.
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
		/// Posici�n respecto al padre.
		/// </summary>
		Vector2 PositionOffset;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector2 ScaleOffset;

		/// <summary>
		/// Rotaci�n respecto al padre.
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
