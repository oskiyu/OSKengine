#pragma once

#include "NumericTypes.h"

namespace OSK::GRAPHICS {

	/// @brief En modo bind-less, n�mero m�ximo de elementos
	/// que se pueden enlazar a un array sin l�mite
	/// definido en shaders (resource[]).
	constexpr USize64 MAX_BINDLESS_RESOURCES = 4096u;

}
