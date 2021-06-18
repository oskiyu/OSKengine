#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Representa c�mo se colocar�n los elementos ahijados, 
	/// al a�adirlos al elemento padre.
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
		/// Se colocan en la posici�n original del hijo.
		/// Compatible con UiAnchor.
		/// </summary>
		FIXED

	};

}