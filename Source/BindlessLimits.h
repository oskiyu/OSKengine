#pragma once

#include "NumericTypes.h"

namespace OSK::GRAPHICS {

	/// @brief En modo bind-less, número máximo de elementos
	/// que se pueden enlazar a un array sin límite
	/// definido en shaders (resource[]).
	constexpr USize64 MAX_BINDLESS_RESOURCES = 4096u;

}
