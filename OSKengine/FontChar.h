#pragma once

#include "Sprite.h"
#include "Vector2.h"

namespace OSK {

	//Carácter de una fuente.
	struct OSKAPI_CALL FontChar {

		//Textura del carácter.
		Sprite sprite{};

		//Tamaño del carácter.
		Vector2 Size;
		Vector2 Bearing;

		uint32_t Advance = 0;

	};

}