#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Representa cómo se colocarán los elementos ahijados, 
	/// al añadirlos al elemento padre.
	/// </summary>
	enum class UiLayout {

		/// <summary>
		/// Se colocan en columna.
		/// </summary>
		VERTICAL,

		/// <summary>
		/// Se colocan en fila.
		/// </summary>
		HORIZONTAL,

		/// <summary>
		/// Se colocan en la posición original del hijo.
		/// Compatible con UiAnchor.
		/// </summary>
		FIXED

	};

}