#pragma once

#include "GpuHeapMemoryUsageInfo.h"
#include "DynamicArray.hpp"

namespace OSK::GRAPHICS {

	/// @brief Información general sobre el uso de memoria GPU de la aplicación.
	/// Puede usarse para saber cuánto espacio se está usando, 
	/// y cúanto queda disponible.
	struct GpuMemoryUsageInfo {

		struct Entry {

			/// @brief Capacidad máxima del tipo de memoria, en bytes.
			USize64 maxCapacity = 0;

			/// @brief Espacio usado del tipo de memoria, en bytes.
			USize64 usedSpace = 0;

		};

		/// @brief Información sobre la memoria GPU-only.
		Entry gpuOnlyMemoryInfo{};
		/// @brief Información sobre la memoria compartida entre GPU y CPU.
		Entry gpuAndCpuMemoryInfo{};

		/// @brief Información de cada heap individual.
		DynamicArray<GpuHeapMemoryUsageInfo> heapsInfo;

	};

}