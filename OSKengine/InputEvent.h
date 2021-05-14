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
	/// Evento de input que puede repetirse una vez por tick.
	/// </summary>
	struct OSKAPI_CALL InputEvent {

		/// <summary>
		/// Nombre del evento.
		/// </summary>
		std::string eventName = "";

		/// <summary>
		/// Teclas que activan el evento.
		/// </summary>
		std::vector<Key> linkedKeys;

		/// <summary>
		/// Botones del ratón que activan el evento.
		/// </summary>
		std::vector<ButtonCode> linkedButtons;

	};

}