#pragma once

#include "GpuHeapMemoryUsageInfo.h"
#include "DynamicArray.hpp"

namespace OSK::GRAPHICS {

	/// @brief Informaci�n general sobre el uso de memoria GPU de la aplicaci�n.
	/// Puede usarse para saber cu�nto espacio se est� usando, 
	/// y c�anto queda disponible.
	struct GpuMemoryUsageInfo {

		struct {

			/// @brief Capacidad m�xima del tipo de memoria, en bytes.
			TSize maxCapacity = 0;

			/// @brief Espacio usado del tipo de memoria, en bytes.
			TSize usedSpace = 0;

		}
		/// @brief Informaci�n sobre la memoria GPU-only.
		gpuOnlyMemoryInfo{},
		/// @brief Informaci�n sobre la memoria compartida entre GPU y CPU.
		gpuAndCpuMemoryInfo{};

		/// @brief Informaci�n de cada heap individual.
		DynamicArray<GpuHeapMemoryUsageInfo> heapsInfo;

	};

}