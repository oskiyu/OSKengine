#pragma once

#include "OSKmacros.h"

#include <glm.hpp>

namespace OSK {

	/// <summary>
	/// Push constants de un sprite.
	/// </summary>
	struct OSKAPI_CALL PushConst2D {

		/// <summary>
		/// Matriz modelo.
		/// </summary>
		alignas(16) glm::mat4 model;

		/// <summary>
		/// Cámara usada para renderizar el sprite.
		/// </summary>
		alignas(16) glm::mat4 camera = glm::mat4(1.0f);

		/// <summary>
		/// Color del sprite.
		/// </summary>
		alignas(16) glm::vec4 color = glm::vec4(1.0f);

	};

}