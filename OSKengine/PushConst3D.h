#pragma once

#include <glm.hpp>

namespace OSK {

	/// <summary>
	/// Contiene la matriz de modelo de un modelo 3D.
	/// </summary>
	struct OSKAPI_CALL PushConst3D {

		/// <summary>
		/// Matriz del modelo.
		/// </summary>
		alignas(16) glm::mat4 model;

	};

}
