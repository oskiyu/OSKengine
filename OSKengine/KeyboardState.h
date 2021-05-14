#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "KeysEnum.h"
#include "KeyStatesEnum.h"

namespace OSK {

	/// <summary>
	/// Número de teclas soportadas.
	/// </summary>
	constexpr keyCode_t NUMBER_OF_KEYS = 119;
	

	/// <summary>
	/// Almacena el estado del teclado en un determinado instante.
	/// </summary>
	struct OSKAPI_CALL KeyboardState {

		friend class WindowAPI;

	public:

		/// <summary>
		/// Retorna el estado de una tecla dada. (KeyState).
		/// </summary>
		/// <param name="key">Tecla del teclado.</param>
		/// <returns>RELEASED o PRESSED.</returns>
		KeyState GetKeyState(Key key);

		/// <summary>
		/// Comprueba si una tecla está siendo pulsada.
		/// </summary>
		/// <param name="key">Tecla del teclado.</param>
		/// <returns>'true' = PRESSED. 'false' = RELEASED.</returns>
		keyState_t IsKeyDown(Key key);

		/// <summary>
		/// Comprueba si una tecla no está siendo pulsada.
		/// </summary>
		/// <param name="key">Tecla del teclado.</param>
		/// <returns>'true' = RELEASED. 'false' = PRESSED.</returns>
		keyState_t IsKeyUp(Key key);

	private:

		/// <summary>
		/// Almacena los estados de todas las teclas.
		/// </summary>
		keyState_t keyStates[NUMBER_OF_KEYS];

	};

}