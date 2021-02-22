#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "KeysEnum.h"
#include "KeyStatesEnum.h"

namespace OSK {

	//Número de teclas soportadas.
	constexpr keyCode_t NUMBER_OF_KEYS = 119;
	
	//Almacena el estado del teclado en un determinado instante.
	struct OSKAPI_CALL KeyboardState {

	public:

		//Almacena los estados de todas las teclas.
		keyState_t KeyStates[NUMBER_OF_KEYS];

		//Retorna el estado de una tecla dada. (KeyState).
		// RELEASED o PRESSED
		KeyState GetKeyState(Key key);

		//Comprueba si una tecla está siendo pulsada.
		// 'true' = PRESSED
		// 'false' = RELEASED
		keyState_t IsKeyDown(Key key);

		//Comprueba si una tecla no está siendo pulsada.
		// 'true' = RELEASED
		// 'false' = PRESSED
		keyState_t IsKeyUp(Key key);

	private:

	};

}