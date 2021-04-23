#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "KeysEnum.h"
#include "ButtonCodeEnum.h"

#include <vector>

namespace OSK {

	/// <summary>
	/// Evento de input que se ejecuta una sola vez, hasta que se suelta la tecla / botón.
	/// </summary>
	struct OSKAPI_CALL OneTimeInputEvent {

		/// <summary>
		/// Nombre del evento.
		/// </summary>
		std::string EventName = "";

		/// <summary>
		/// Teclas que activan el evento.
		/// </summary>
		std::vector<Key> LinkedKeys;

		/// <summary>
		/// Botones del ratón que activan el evento.
		/// </summary>
		std::vector<ButtonCode> LinkedButtons;

	};


}