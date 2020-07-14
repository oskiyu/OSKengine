#pragma once

#include "Sprite.h"
#include "Vector2.h"

namespace OSK {

	struct OSKAPI_CALL FontChar {

		Sprite sprite{};

		Vector2 Size;
		Vector2 Bearing;

		uint32_t Advance;

	};

}