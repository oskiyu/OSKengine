#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Indica qué hacer cuando el texto sobrepasa el límite de la pantalla.
	/// </summary>
	enum class OSKAPI_CALL TextRenderingLimit {

		/// <summary>
		/// No hacer nada.
		/// </summary>
		DO_NOTHING,

		/// <summary>
		/// Renderizar el resto del texto en una nueva línea.
		/// </summary>
		NEW_LINE,

		/// <summary>
		/// Mueve el texto hacia la izquierda para que se pueda renderizar entero.
		/// </summary>
		MOVE_TEXT

	};

}
