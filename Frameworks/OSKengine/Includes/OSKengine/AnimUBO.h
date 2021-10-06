#pragma once

#include "OSKmacros.h"

#include "Vertex.h"

#include <array>
#include <glm.hpp>

namespace OSK {

	/// <summary>
	/// UBO que contiene informaci�n sobre los huesos de un modelo animado.
	/// </summary>
	struct OSKAPI_CALL AnimUBO {

		/// <summary>
		/// UBO vaci�o.
		/// </summary>
		AnimUBO();

		/// <summary>
		/// Huesos a enviar.
		/// </summary>
		std::array<glm::mat4, OSK_ANIM_MAX_BONES> bones{};

	};

}
