#pragma once

#include "Vector2.hpp"
#include "glm/fwd.hpp"

namespace OSK::GRAPHICS {

	/// @brief Información global del renderizado
	/// SDF.
	struct SdfGlobalInformationBufferContent2D {

		/// @brief Modelo de la cámara 2D.
		alignas(16) glm::mat4 cameraMatrix;

		/// @brief Resolución del área renderizada.
		/// @pre Debe ser la resolución definida por
		/// el viewport.
		alignas(16) Vector2f resolution;

	};

}
