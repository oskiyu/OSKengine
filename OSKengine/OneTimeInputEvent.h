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
	/// Evento de input que se ejecuta una sola vez, hasta que se suelta la tecla / bot�n.
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
		/// Botones del rat�n que activan el evento.
		/// </summary>
		std::vector<ButtonCode> LinkedButtons;

	};


}