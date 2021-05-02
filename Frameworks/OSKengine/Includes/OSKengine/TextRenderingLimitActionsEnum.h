#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Indica qu� hacer cuando el texto sobrepasa el l�mite de la pantalla.
	/// </summary>
	enum class OSKAPI_CALL TextRenderingLimit {

		/// <summary>
		/// No hacer nada.
		/// </summary>
		DO_NOTHING,

		/// <summary>
		/// Renderizar el resto del texto en una nueva l�nea.
		/// </summary>
		NEW_LINE,

		/// <summary>
		/// Mueve el texto hacia la izquierda para que se pueda renderizar entero.
		/// </summary>
		MOVE_TEXT

	};

}
