#pragma once

#include "NumericTypes.h"


namespace OSK::GRAPHICS {

	/// @brief Rango de información de un buffer.
	struct GpuBufferRange {

		static GpuBufferRange CreateFullRange() {
			return GpuBufferRange{};
		}

		/// @brief Offset respecto al inicio del buffer.
		UIndex64 offset = 0;

		/// @brief Tamaño, en bytes.
		USize64 size = 0;

		/// @brief True si es todo el buffer.
		bool full = true;

	};

}
