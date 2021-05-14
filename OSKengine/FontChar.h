#pragma once

#include "Sprite.h"
#include "Vector2.hpp"

namespace OSK {

	/// <summary>
	/// Car�cter de una fuente.
	/// </summary>
	struct OSKAPI_CALL FontChar {

		/// <summary>
		/// Sprite del car�cter.
		/// </summary>
		Sprite sprite{};

		/// <summary>
		/// Tama�o del car�cter.
		/// </summary>
		Vector2 size;

		/// <summary>
		/// Espaciado del car�cter.
		/// </summary>
		Vector2 bearing;

		/// <summary>
		/// Tama�o horizontal del car�cter.
		/// </summary>
		uint32_t advance = 0;

	};

}