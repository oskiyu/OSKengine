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
		//Posición, escala y rotación.
		Transform2D();

		//Clase que almacena el 'transform' de un objeto en un mundo 3D.
		//Posición, escala y rotación.
		Transform2D(const Vector2& position, const Vector2& scale, float rotation);

		//Destruye el transform.
		~Transform2D();

		//Establece la posición del transform.
		//También actualiza la matriz modelo.
		void SetPosition(const Vector2& position);

		//Establece la escala del transform.
		//También actualiza la matriz modelo.
		void SetScale(const Vector2& scale);

		//Establece la rotación del transform.
		//También actualiza la matriz modelo.
		void SetRotation(float scale);

		//Cambia la posición del transform.
		//También actualiza la matriz modelo.
		void AddPosition(const Vector2& positionDelta);

		//Cambia la escala del transform.
		//También actualiza la matriz modelo.
		void AddScale(const Vector2& scaleDelta);

		//Cambia la rotación del transform.
		//También actualiza la matriz modelo.
		void AddRotation(float rotationDelta);

		//Actualiza la matriz modelo.
		//La matriz modelo es la información que se le envía a la GPU.
		void UpdateModel();

		//Establece el 'transform' baseTransform como base.
		void AttachTo(Transform2D* baseTransform);

		//Elimina el 'transform' base.
		void UnAttach();

		//Devuelve un rectángulo con la posición y el tamaño de este transform.
		Vector4 GetRectangle() const;


		//Vector posición en el mundo 2D.
		Vector2 GlobalPosition;

		//Vector escala en el mundo 2D.
		Vector2 GlobalScale;

		//Rotación en el mundo 2D.
		float GlobalRotation;

		//Vector posición.
		Vector2 Position;

		//Vector escala.
		Vector2 Scale;

		//Vector rotación.
		float Rotation;

		//Matriz "model".
		glm::mat4 ModelMatrix;

		//Transformación padre.
		Transform2D* ParentTransform;

		//Transformaciones ahijadas.
		std::vector<Transform2D*> ChildTransforms;

		//Offset de la posición (del transform padre).
		Vector2 PositionOffset;

		//Offset de la escala (del transform padre).
		Vector2 ScaleOffset;

		//Offset de la rotación (del transform padre).
		float RotationOffset;

		//Establece si ha de heredar la escala padre.
		bool UseParentScale = false;

	private:

		bool isAttached = false;
		bool isParent = false;

		Transform2D* parent = nullptr;

	};

}
