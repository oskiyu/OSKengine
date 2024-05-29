#pragma once

#include "NumericTypes.h"
#include "GpuMemoryTypes.h"

namespace OSK::GRAPHICS {

	/// @brief Informaci�n sobre el uso de un heap de memoria
	/// de la GPU.
	/// Puede usarse para saber cu�nto espacio se est� usando, 
	/// y c�anto queda disponible.
	struct GpuHeapMemoryUsageInfo {

		/// @brief Capacidad m�xima del heap, en bytes.
		USize64 maxCapacity;

		/// @brief Espacio usado, en bytes.
		USize64 usedSpace;

		/// @brief Tipo de memoria.
		GpuSharedMemoryType memoryType;

	};

}
