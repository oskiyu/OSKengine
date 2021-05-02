#pragma once

#include <glm.hpp>
#include <vector>

namespace OSK{

	/// <summary>
	/// Estructura que pasa información a la GPU para el renderizado 3D.
	/// </summary>
	struct OSKAPI_CALL UBO {

		/// <summary>
		/// Matriz view de la cámara 3D.
		/// </summary>
		alignas(16) glm::mat4 view;

		/// <summary>
		/// Matriz Proyección de la cámara 3D.
		/// </summary>
		alignas(16) glm::mat4 projection;

		/// <summary>
		/// Posición de la cámara 3D.
		/// </summary>
		/// 
		alignas(16) glm::vec3 cameraPos;

	};

}
