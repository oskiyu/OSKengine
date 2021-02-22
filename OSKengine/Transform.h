#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <glm.hpp>
#include "Quaternion.h"

#include <vector>

#include <assimp/matrix4x4.h>

namespace OSK {

	//Clase que almacena el 'transform' de un objeto en un mundo 3D.
	//Posición, escala y rotación.
	class OSKAPI_CALL Transform {

	public:

		//Crea la clase.
		Transform();

		//Crea la clase con la posición, escala y rotación establecidos.
		Transform(const Vector3f& position, const Vector3f& scale, const Vector3f& rotation);
			
		//Destruye la clase.
		~Transform();

		//Establece la posición.
		//También actualiza la matriz modelo.
		void SetPosition(const Vector3f& position);

		//Establece la escala.
		//También actualiza la matriz modelo.
		void SetScale(const Vector3f& scale);

		//Establece la rotación.
		//También actualiza la matriz modelo.
		void SetRotation(const Quaternion& rotation);

		//Suma un vector 3D a la posición.
		void AddPosition(const Vector3f& positionDelta);

		//Suma un vector 3D a la escala.
		void AddScale(const Vector3f& scaleDelta);

		//Suma un quaternion a la rotación.
		void ApplyRotation(const Quaternion& rotationDelta);

		//Rota el transform, usando un eje en espacio local.
		void RotateLocalSpace(float angle, const Vector3f& axis);

		//Rota el transform, usando un eje en espacio global.
		void RotateWorldSpace(float angle, const Vector3f& axis);

		//Actualiza la matriz modelo.
		void UpdateModel();

		//Establece el 'transform' baseTransform como base.
		void AttachTo(Transform* baseTransform);

		//Elimina el 'transform' base.
		void UnAttach();

		//Vector posición en el mundo 3D.
		Vector3f GlobalPosition;

		//Vector escala en el mundo 3D.
		Vector3f GlobalScale;

		//Vector posición local.
		Vector3f LocalPosition;

		//Vector escala local.
		Vector3f LocalScale;

		//Orientación.
		Quaternion Rotation;

		//Matriz "model".
		glm::mat4 ModelMatrix;
	
		//Transformación padre.
		Transform* ParentTransform;

		//Transformaciones ahijadas.
		std::vector<Transform*> ChildTransforms;

	private:

		static glm::mat4 toGlmMat(const aiMatrix4x4* src);

		bool isAttached = false;
		bool isParent = false;

	};

}