#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ButtonStatesEnum.h"
#include "ButtonCodeEnum.h"

namespace OSK {

	//Número de botones soportados.
	constexpr buttonCode_t NUMBER_OF_BUTTONS = 8;

	//Struct que almacena el estado del ratón en un determinado momento.
	struct OSKAPI_CALL MouseState {

	public:

		//Posición en X, en píxeles, respecto a la esquina superior izquierda.
		double PositionX;
		//Posición en Y, en píxeles, respecto a la esquina superior izquierda.
		double PositionY;

		//Posición en X, en porcentaje, reespecto a la esquina superior izquierda.
		//Entre 0.0 y 1.0
		//0.0 = borde izquierdo.
		//1.0 = borde derecho.
		double RelativePositionX;
		//Posición en X, en porcentaje, reespecto a la esquina superior izquierda.
		//Entre 0.0 y 1.0
		//0.0 = borde izquierdo.
		//1.0 = borde derecho.
		double RelativePositionY;

		//Rueda del ratón horizontal.
		float_t ScrollX = 0.0f;
		//Rueda del ratón vertical (la normal).
		float_t ScrollY = 0.0f;

		float_t OldScrollX = 0.0f;
		float_t OldScrollY = 0.0f;

		//Estado de los botones.
		//'true' = pressed.
		//'false' = released.
		bool ButtonStates[NUMBER_OF_BUTTONS];

		//Obtiene el estado de un botón.
		ButtonState GetButtonState(ButtonCode button) const;

		//Obtiene si un botón está siendo pulsado.
		bool IsButtonDown(ButtonCode button) const;
		
		//Obtiene si un botón no está siendo pulsado
		bool IsButtonUp(ButtonCode button) const;

		Vector4 GetMouseRectangle(float size = 2.0f) const;

	private:

	};

}