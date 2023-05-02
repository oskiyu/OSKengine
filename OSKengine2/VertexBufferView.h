#pragma once

#include "VertexInfo.h"

namespace OSK::GRAPHICS {

	/// @brief Estructura que permite interpretar los datos
	/// de un vertex buffer.
	struct OSKAPI_CALL VertexBufferView final {

		/// @brief Informaci�n del tipo de v�rtice.
		VertexInfo vertexInfo{};

		/// @brief N�mero de v�rtices.
		TSize numVertices = 0;

		/// @brief Offset del primer v�rtice respecto al inicio
		/// del buffer.
		TSize offsetInBytes= 0;

	};

}
