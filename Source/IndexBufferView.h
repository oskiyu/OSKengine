#pragma once

#include "OSKmacros.h"
#include "NotImplementedException.h"

namespace OSK::GRAPHICS {

	/// @brief Formatos de índice disponibles.
	enum class IndexType {
		/// @brief Un índice es un único byte.
		U8,
		/// @brief Índices de dos bytes.
		U16,
		/// @brief Índices de cuatro bytes.
		U32
	};

	/// @brief Obtiene el formato de índicedependiendo del tipo de dato.
	/// @tparam T Tipo de dato.
	/// @return Tipo de vértice.
	/// 
	/// @throws std::runtime_error si el tipo de dato no representa ningún formato de índice válido.
	template <typename T>
	IndexType GetIndexType() {
		switch (sizeof(T)) {
		case 1: return IndexType::U8;
		case 2: return IndexType::U16;
		case 4: return IndexType::U32;
		default: throw NotImplementedException();
		}
	}

	/// @brief Estructura que permite interpretar los datos
	/// de un index buffer.
	struct OSKAPI_CALL IndexBufferView final {

		/// @brief Tipo de dato de los índices.
		IndexType type;

		/// @brief Número de índices.
		USize32 numIndices = 0;

		/// @brief Offset del primer índice respecto al inicio
		/// del buffer.
		USize32 offsetInBytes = 0;
	};

}
