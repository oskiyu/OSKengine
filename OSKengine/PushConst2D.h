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
		/// Color del sprite.
		/// </summary>
		alignas(16) glm::vec4 color = glm::vec4(1.0f);

		alignas(16) glm::vec4 texCoords = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	};

}
