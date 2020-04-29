#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <glm.hpp>

#include <string>
#include <array>
#include <vector>

#include <assimp/matrix4x4.h>

namespace OSK {

	//Clase que almacena el 'transform' de un objeto en un mundo 3D.
	//Posición, escala y rotación.
	class Transform {

	public:

		//Crea la clase.
		Transform();


		//Crea la clase con la posición, escala y rotación establecidos.
		Transform(const Vector3& position, const Vector3& scale, const Vector3& rotation);


		//Crea la clase desde una matriz.
		Transform(const glm::mat4& model);


		//Destruye la clase.
		~Transform();


		//Establece la posición.
		//También actualiza la matriz modelo.
		void SetPosition(const Vector3& position);


		//Establece la escala.
		//También actualiza la matriz modelo.
		void SetScale(const Vector3& rotation);


		//Establece la rotación.
		//También actualiza la matriz modelo.
		void SetRotation(const Vector3& scale);


		//Suma un vector 3D a la posición.
		void AddPosition(const Vector3& positionDelta);


		//Suma un vector 3D a la escala.
		void AddScale(const Vector3& scaleDelta);


		//Suma un vector 3D a la rotación.
		void AddRotation(const Vector3& rotationDelta);


		//Actualiza la matriz modelo.
		void UpdateModel();


		//Establece el 'transform' baseTransform como base.
		void AttachTo(Transform* baseTransform);


		//Elimina el 'transform' base.
		void UnAttach();


		//Vector posición en el mundo 3D.
		Vector3 GlobalPosition;


		//Vector escala en el mundo 3D.
		Vector3 GlobalScale;


		//Vector rotación en el mundo 3D.
		Vector3 GlobalRotation;


		//Vector posición.
		Vector3 Position;


		//Vector escala.
		Vector3 Scale;


		//Vector rotación.
		Vector3 Rotation;


		//Establece si ha de usarse la matriz modelo.
		//Solo true en casos de modelos 3D.
		bool UseModelMatrix = false;


		//Matriz "model".
		OSK_INFO_READ_ONLY
			glm::mat4 ModelMatrix;
	

		//Transformación padre.
		Transform* ParentTransform;


		//Transformaciones ahijadas.
		std::vector<Transform*> ChildTransforms;


		//Offset de la posición (del transform padre).
		Vector3 PositionOffset;


		//Offset de la rotación (del transform padre).
		Vector3 RotationOffset;


		//Offset de la escala (del transform padre).
		Vector3 ScaleOffset;


		//Establece si ha de heredar la escala padre.
		bool UseParentScale = false;

	private:

		void toGlmMat(const aiMatrix4x4* src, glm::mat4& dst);

		bool isAttached = false;

		bool isParent = false;

		bool isSkeletal = false;

	};
}