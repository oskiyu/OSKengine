#pragma once

#include "VertexInfo.h"

namespace OSK::GRAPHICS {

	/// @brief Estructura que permite interpretar los datos
	/// de un vertex buffer.
	struct OSKAPI_CALL VertexBufferView final {

		/// @brief Información del tipo de vértice.
		VertexInfo vertexInfo{};

		/// @brief Número de vértices.
		TSize numVertices = 0;

		/// @brief Offset del primer vértice respecto al inicio
		/// del buffer.
		TSize offsetInBytes= 0;

	};

}
