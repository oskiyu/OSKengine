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
		std::string eventName = "";

		/// <summary>
		/// Teclas que activan el evento.
		/// </summary>
		std::vector<Key> linkedKeys;

		/// <summary>
		/// Botones del rat�n que activan el evento.
		/// </summary>
		std::vector<ButtonCode> linkedButtons;

		/// <summary>
		/// Botones del gamepad que activan el evento.
		/// </summary>
		std::vector<GamepadButton> linkedGamepadButtons;

		/// <summary>
		/// Gamepad que lo controla.
		/// </summary>
		int gamepadCode = 0;

	};


}