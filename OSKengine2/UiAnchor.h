#pragma once

#include "EnumFlags.hpp"

namespace OSK::UI {

	enum class Anchor {
		TOP			= 1 << 0,
		CENTER_X	= 1 << 1,
		BOTTOM		= 1 << 2,
		LEFT		= 1 << 3,
		CENTER_Y	= 1 << 4,
		RIGHT		= 1 << 5,

		FULLY_CENTERED = CENTER_X | CENTER_Y,
		DEFAULT = FULLY_CENTERED
	};

}

OSK_FLAGS(OSK::UI::Anchor);
