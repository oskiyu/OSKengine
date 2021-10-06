#pragma once

#include <glm.hpp>
#include <vector>

#include "PointLight.h"
#include "DirectionalLight.h"

namespace OSK{

	/// <summary>
	/// Estructura que pasa informaci�n a la GPU para el renderizado 3D.
	/// </summary>
	struct OSKAPI_CALL UboCamera3D {

		/// <summary>
		/// Matriz view de la c�mara 3D.
		/// </summary>
		alignas(16) glm::mat4 view;

		/// <summary>
		/// Matriz Proyecci�n de la c�mara 3D.
		/// </summary>
		alignas(16) glm::mat4 projection;

		/// <summary>
		/// Posici�n de la c�mara 3D.
		/// </summary>
		/// 
		alignas(16) glm::vec3 cameraPos;

	};

}
