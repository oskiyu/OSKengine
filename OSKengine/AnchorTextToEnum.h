#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Anclaje del texto: 
	/// respecto a la pantalla o respecto a la interfaz de usuario.
	/// </summary>
	enum class OSKAPI_CALL AnchorTextTo {

		/// <summary>
		/// (Por defecto).
		/// La posición está establecida respecto a la pantalla.
		/// </summary>
		SCREEN,

		/// <summary>
		/// La posición está establecida respecto al elemento de interfaz de usuario.
		/// </summary>
		UI_ELEMENT

	};

}
