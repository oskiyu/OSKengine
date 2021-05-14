#pragma once

#include "OSKmacros.h"

#include <glm.hpp>

namespace OSK {

	/// <summary>
	/// El propio hueso.
	/// Contiene las matrices del hueso.
	/// </summary>
	struct OSKAPI_CALL BoneInfo {

		/// <summary>
		/// Offset del hueso.
		/// </summary>
		glm::mat4 offset = glm::mat4(1.0f);

		/// <summary>
		/// Transformación del hueso.
		/// </summary>
		glm::mat4 finalTransformation = glm::mat4(1.0f);

	};

}