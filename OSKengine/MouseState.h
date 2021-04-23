#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ButtonStatesEnum.h"
#include "ButtonCodeEnum.h"

namespace OSK {

	/// <summary>
	/// N�mero de botones soportados.
	/// </summary>
	constexpr buttonCode_t NUMBER_OF_BUTTONS = 8;


	/// <summary>
	/// Struct que almacena el estado del rat�n en un determinado momento.
	/// </summary>
	struct OSKAPI_CALL MouseState {

	public:

		/// <summary>
		/// Posici�n en X, en p�xeles, respecto a la esquina superior izquierda.
		/// </summary>
		mouseVar_t PositionX;

		/// <summary>
		/// Posici�n en Y, en p�xeles, respecto a la esquina superior izquierda.
		/// </summary>
		mouseVar_t PositionY;

		/// <summary>
		/// Posici�n en X, en porcentaje, reespecto a la esquina superior izquierda.
		/// Entre 0.0 y 1.0.
		/// 0.0 = borde izquierdo.
		/// 1.0 = borde derecho.
		/// </summary>
		mouseVar_t RelativePositionX;
		
		/// <summary>
		/// Posici�n en Y, en porcentaje, reespecto a la esquina superior izquierda.
		/// Entre 0.0 y 1.0.
		/// 0.0 = borde izquierdo.
		/// 1.0 = borde derecho.
		/// </summary>
		mouseVar_t RelativePositionY;

		/// <summary>
		/// Rueda del rat�n horizontal.
		/// </summary>
		float_t ScrollX = 0.0f;

		/// <summary>
		/// Rueda del rat�n vertical (la normal).
		/// </summary>
		float_t ScrollY = 0.0f;

		/// <summary>
		/// Rueda del rat�n horizontal en el anterior frame.
		/// </summary>
		float_t OldScrollX = 0.0f;

		/// <summary>
		/// Rueda del rat�n vertical (la normal) en el anterior frame.
		/// </summary>
		float_t OldScrollY = 0.0f;

		/// <summary>
		/// Estado de los botones. 
		/// 'true' = pressed. 
		/// 'false' = released.
		/// </summary>
		bool ButtonStates[NUMBER_OF_BUTTONS];

		/// <summary>
		/// Obtiene el estado de un bot�n.
		/// </summary>
		/// <param name="button">Bot�n del rat�n.</param>
		/// <returns>Estado.</returns>
		ButtonState GetButtonState(ButtonCode button) const;

		//Obtiene si un bot�n est� siendo pulsado.

		/// <summary>
		/// Obtiene si un bot�n est� siendo pulsado.
		/// </summary>
		/// <param name="button">Bot�n del rat�n.</param>
		/// <returns>Estado.</returns>
		bool IsButtonDown(ButtonCode button) const;
		
		//Obtiene si un bot�n no est� siendo pulsado

		/// <summary>
		/// Obtiene si un bot�n no est� siendo pulsado.
		/// </summary>
		/// <param name="button">Bot�n del rat�n.</param>
		/// <returns>Estado.</returns>
		bool IsButtonUp(ButtonCode button) const;

		/// <summary>
		/// Obtiene un rect�ngulo alrededor del cursor.
		/// </summary>
		/// <param name="size">Ancho y alto del rect�ngulo.</param>
		/// <returns>Rect�ngulo.</returns>
		Vector4 GetMouseRectangle(float size = 2.0f) const;

	};

}