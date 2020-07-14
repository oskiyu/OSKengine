#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Animation.h"
#include "Bone.h"
#include "Skeleton.h"
#include "Mesh.h"
#include "OldTexture.h"
#include "Transform.h"

#include <glm.hpp>

#include <string>
#include <map>
#include <vector>

namespace OSK {

	//Modelo 3D.
	class OSKAPI_CALL Model {

	public:

		//Establece un modelo 3D.
		//Tamaño: size (escala en { x, y, z } por separado).
		//Rotación: rotation { x, y, z }
		Model(const Vector3& position, const Vector3& size, const Vector3& rotation);


		//Destruye el modelo.
		~Model();


		//Establece la posición del modelo.
		//También actualiza la matriz del modelo.
		void SetPosition(const Vector3& position);


		//Establece el tamaño del modelo.
		//También actualiza la matriz del modelo.
		void SetScale(const Vector3& size);


		//Establece la rotación del modelo.
		//También actualiza la matriz del modelo.
		void SetRotation(const Vector3& rotation);


		//Textura del modelo.
		//Textura principal.
		OldTexture Diffuse;


		//Textura del modelo.
		//Textura del "brillo".
		OldTexture Specular;


		//Esqueleto del modelo.
		OSK_INFO_DO_NOT_TOUCH
			Skeleton Skeleton;


		//Mallas del modelo.
		OSK_INFO_DO_NOT_TOUCH
			std::vector<Mesh> Meshes;


		//Matriz del modelo.
		//   Se actualiza sola al usar las siguientes funciones:
		//-SetPosition();
		//-SetSize();
		//-SetRotation();
		OSK_INFO_READ_ONLY
			Transform ModelTransform;


		//True si el modelo ha sido cargado y está listo para usarse.
		bool IsLoaded = false;


		//True si el modelo usa animaciones.
		bool Skeletal = false;

	};

}