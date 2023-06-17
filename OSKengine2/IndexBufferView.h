#pragma once

#include "OSKmacros.h"
#include "NotImplementedException.h"

namespace OSK::GRAPHICS {

	/// @brief Formatos de �ndice disponibles.
	enum class IndexType {
		/// @brief Un �ndice es un �nico byte.
		U8,
		/// @brief �ndices de dos bytes.
		U16,
		/// @brief �ndices de cuatro bytes.
		U32
	};

	/// @brief Obtiene el formato de �ndicedependiendo del tipo de dato.
	/// @tparam T Tipo de dato.
	/// @return Tipo de v�rtice.
	/// 
	/// @throws std::runtime_error si el tipo de dato no representa ning�n formato de �ndice v�lido.
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

		/// @brief Tipo de dato de los �ndices.
		IndexType type;

		/// @brief N�mero de �ndices.
		USize32 numIndices = 0;

		/// @brief Offset del primer �ndice respecto al inicio
		/// del buffer.
		USize32 offsetInBytes = 0;
	};

}
