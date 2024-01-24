#pragma once

#include "OSKmacros.h"

namespace OSK::PERSISTENCE {

	/// @brief Índice que indica un tipo de dato.
	using TDataType = UIndex64;

	constexpr inline TDataType DATA_TYPE_UNKNOWN = 0;
	constexpr inline TDataType DATA_TYPE_STRING  = 1;
	constexpr inline TDataType DATA_TYPE_INT	 = 2;
	constexpr inline TDataType DATA_TYPE_FLOAT	 = 3;
	constexpr inline TDataType DATA_TYPE_RESERVED_MAX = 100;

}
