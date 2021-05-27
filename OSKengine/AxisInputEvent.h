#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ButtonCodeEnum.h"

#include <vector>

#include "GamepadState.h"
#include "MouseState.h"

namespace OSK {

	/// <summary>
	/// Estructura para poder usar teclas en eventos de eje.
	/// </summary>
	struct KeyToAxis {

		/// <summary>
		/// Tecla 1.
		/// </summary>
		Key keyA = Key::ESCAPE;

		/// <summary>
		/// Tecla 2.
		/// </summary>
		Key keyB = Key::ESCAPE;

		/// <summary>
		/// Valor del primer botón en el eje.
		/// </summary>
		const float valueA = 1.0f;

		/// <summary>
		/// Valor del segundo botón en el eje.
		/// </summary>
		const float valueB = -1.0f;

	};

	/// <summary>
	/// Un evento de input que puede variar de intensidad.
	/// </summary>
	struct OSKAPI_CALL AxisInputEvent {

		/// <summary>
		/// Nombre del evento.
		/// </summary>
		std::string eventName = "";

		/// <summary>
		/// Ejes (de botones).
		/// </summary>
		std::vector<KeyToAxis> linkedGamepadAxes;

		/// <summary>
		/// Ejes.
		/// </summary>
		std::vector<GamepadAxis> linkedAxes;

		/// <summary>
		/// Mando que controla el evento.
		/// </summary>
		int gamepadCode = 0;

	};

}
