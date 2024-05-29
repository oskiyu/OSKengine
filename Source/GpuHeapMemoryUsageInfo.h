#pragma once

#include "NumericTypes.h"
#include "GpuMemoryTypes.h"

namespace OSK::GRAPHICS {

	/// @brief Información sobre el uso de un heap de memoria
	/// de la GPU.
	/// Puede usarse para saber cuánto espacio se está usando, 
	/// y cúanto queda disponible.
	struct GpuHeapMemoryUsageInfo {

		/// @brief Capacidad máxima del heap, en bytes.
		USize64 maxCapacity;

		/// @brief Espacio usado, en bytes.
		USize64 usedSpace;

		/// @brief Tipo de memoria.
		GpuSharedMemoryType memoryType;

	};

}
