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
		Vector2 size;

		/// <summary>
		/// Espaciado del carácter.
		/// </summary>
		Vector2 bearing;

		/// <summary>
		/// Tamaño horizontal del carácter.
		/// </summary>
		uint32_t advance = 0;

	};

}