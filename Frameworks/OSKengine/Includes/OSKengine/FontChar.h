#pragma once

#include "Sprite.h"
#include "Vector2.hpp"

namespace OSK {

	/// <summary>
	/// Carácter de una fuente.
	/// </summary>
	struct OSKAPI_CALL FontChar {

		/// <summary>
		/// Sprite del carácter.
		/// </summary>
		Sprite sprite{};

		/// <summary>
		/// Tamaño del carácter.
		/// </summary>
		Vector2 Size;

		/// <summary>
		/// Espaciado del carácter.
		/// </summary>
		Vector2 Bearing;

		/// <summary>
		/// Tamaño horizontal del carácter.
		/// </summary>
		uint32_t Advance = 0;

	};

}