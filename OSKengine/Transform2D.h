#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vector>

namespace OSK {

	class OSKAPI_CALL Transform2D {

	public:

		//Clase que almacena el 'transform' de un objeto en un mundo 3D.
		//Posici�n, escala y rotaci�n.
		Transform2D();

		//Clase que almacena el 'transform' de un objeto en un mundo 3D.
		//Posici�n, escala y rotaci�n.
		Transform2D(const Vector2& position, const Vector2& scale, float rotation);

		//Destruye el transform.
		~Transform2D();

		//Establece la posici�n del transform.
		//Tambi�n actualiza la matriz modelo.
		void SetPosition(const Vector2& position);

		//Establece la escala del transform.
		//Tambi�n actualiza la matriz modelo.
		void SetScale(const Vector2& scale);

		//Establece la rotaci�n del transform.
		//Tambi�n actualiza la matriz modelo.
		void SetRotation(float scale);

		//Cambia la posici�n del transform.
		//Tambi�n actualiza la matriz modelo.
		void AddPosition(const Vector2& positionDelta);

		//Cambia la escala del transform.
		//Tambi�n actualiza la matriz modelo.
		void AddScale(const Vector2& scaleDelta);

		//Cambia la rotaci�n del transform.
		//Tambi�n actualiza la matriz modelo.
		void AddRotation(float rotationDelta);

		//Actualiza la matriz modelo.
		//La matriz modelo es la informaci�n que se le env�a a la GPU.
		void UpdateModel();

		//Establece el 'transform' baseTransform como base.
		void AttachTo(Transform2D* baseTransform);

		//Elimina el 'transform' base.
		void UnAttach();

		//Devuelve un rect�ngulo con la posici�n y el tama�o de este transform.
		Vector4 GetRectangle() const;


		//Vector posici�n en el mundo 2D.
		Vector2 GlobalPosition;

		//Vector escala en el mundo 2D.
		Vector2 GlobalScale;

		//Rotaci�n en el mundo 2D.
		float GlobalRotation;

		//Vector posici�n.
		Vector2 Position;

		//Vector escala.
		Vector2 Scale;

		//Vector rotaci�n.
		float Rotation;

		//Matriz "model".
		glm::mat4 ModelMatrix;

		//Transformaci�n padre.
		Transform2D* ParentTransform;

		//Transformaciones ahijadas.
		std::vector<Transform2D*> ChildTransforms;

		//Offset de la posici�n (del transform padre).
		Vector2 PositionOffset;

		//Offset de la escala (del transform padre).
		Vector2 ScaleOffset;

		//Offset de la rotaci�n (del transform padre).
		float RotationOffset;

		//Establece si ha de heredar la escala padre.
		bool UseParentScale = false;

	private:

		bool isAttached = false;
		bool isParent = false;

		Transform2D* parent = nullptr;

	};

}
