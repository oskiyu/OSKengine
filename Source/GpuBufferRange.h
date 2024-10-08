#pragma once

#include "NumericTypes.h"


namespace OSK::GRAPHICS {

	/// @brief Rango de informaci�n de un buffer.
	struct GpuBufferRange {

		static GpuBufferRange CreateFullRange() {
			return GpuBufferRange{};
		}

		/// @brief Offset respecto al inicio del buffer.
		UIndex64 offset = 0;

		/// @brief Tama�o, en bytes.
		USize64 size = 0;

		/// @brief True si es todo el buffer.
		bool full = true;

	};

}
