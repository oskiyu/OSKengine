#pragma once

#include "Sprite.h"
#include "Vector2.h"

namespace OSK {

	//Car�cter de una fuente.
	struct OSKAPI_CALL FontChar {

		//Textura del car�cter.
		Sprite sprite{};

		//Tama�o del car�cter.
		Vector2 Size;
		Vector2 Bearing;

		uint32_t Advance = 0;

	};

}