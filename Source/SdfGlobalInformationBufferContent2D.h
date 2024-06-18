#pragma once

#include "Vector2.hpp"
#include "glm/fwd.hpp"

namespace OSK::GRAPHICS {

	/// @brief Informaci�n global del renderizado
	/// SDF.
	struct SdfGlobalInformationBufferContent2D {

		/// @brief Modelo de la c�mara 2D.
		alignas(16) glm::mat4 cameraMatrix;

		/// @brief Resoluci�n del �rea renderizada.
		/// @pre Debe ser la resoluci�n definida por
		/// el viewport.
		alignas(16) Vector2f resolution;

	};

}
