#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ButtonStatesEnum.h"
#include "ButtonCodeEnum.h"

namespace OSK {

	//N�mero de botones soportados.
	constexpr buttonCode_t NUMBER_OF_BUTTONS = 8;

	//Struct que almacena el estado del rat�n en un determinado momento.
	struct OSKAPI_CALL MouseState {

	public:

		//Posici�n en X, en p�xeles, respecto a la esquina superior izquierda.
		double PositionX;
		//Posici�n en Y, en p�xeles, respecto a la esquina superior izquierda.
		double PositionY;

		//Posici�n en X, en porcentaje, reespecto a la esquina superior izquierda.
		//Entre 0.0 y 1.0
		//0.0 = borde izquierdo.
		//1.0 = borde derecho.
		double RelativePositionX;
		//Posici�n en X, en porcentaje, reespecto a la esquina superior izquierda.
		//Entre 0.0 y 1.0
		//0.0 = borde izquierdo.
		//1.0 = borde derecho.
		double RelativePositionY;

		//Rueda del rat�n horizontal.
		float_t ScrollX = 0.0f;
		//Rueda del rat�n vertical (la normal).
		float_t ScrollY = 0.0f;

		float_t OldScrollX = 0.0f;
		float_t OldScrollY = 0.0f;

		//Estado de los botones.
		//'true' = pressed.
		//'false' = released.
		bool ButtonStates[NUMBER_OF_BUTTONS];

		//Obtiene el estado de un bot�n.
		ButtonState GetButtonState(ButtonCode button) const;

		//Obtiene si un bot�n est� siendo pulsado.
		bool IsButtonDown(ButtonCode button) const;
		
		//Obtiene si un bot�n no est� siendo pulsado
		bool IsButtonUp(ButtonCode button) const;

		Vector4 GetMouseRectangle(float size = 2.0f) const;

	private:

	};

}