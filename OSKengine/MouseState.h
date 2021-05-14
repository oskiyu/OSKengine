#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ButtonStatesEnum.h"
#include "ButtonCodeEnum.h"

namespace OSK {

	/// <summary>
	/// Número de botones soportados.
	/// </summary>
	constexpr buttonCode_t NUMBER_OF_BUTTONS = 8;


	/// <summary>
	/// Struct que almacena el estado del ratón en un determinado momento.
	/// </summary>
	struct OSKAPI_CALL MouseState {

		friend class WindowAPI;

	public:

		/// <summary>
		/// Posición en píxeles respecto a la esquina superior izquierda.
		/// </summary>
		Vector2f GetPosition() const;

		/// <summary>
		/// Posición en porcentaje respecto a la esquina superior izquierda de la ventana.
		/// Entre 0.0 y 1.0.
		/// 0.0 = borde izquierdo.
		/// 1.0 = borde derecho.
		/// </summary>
		Vector2f GetRelativePosition() const;
		
		/// <summary>
		/// Rueda del ratón horizontal.
		/// </summary>
		float GetScrollX() const;

		/// <summary>
		/// Rueda del ratón vertical (la normal).
		/// </summary>
		float GetScrollY() const;

		/// <summary>
		/// Cambio de rueda del ratón horizontal desde el anterior frame.
		/// </summary>
		float GetScrollDeltaX() const;

		/// <summary>
		/// Cambio de rueda del ratón vertical (la normal) desde el anterior frame.
		/// </summary>
		float GetScrollDeltaY() const;

		/// <summary>
		/// Obtiene el estado de un botón.
		/// </summary>
		/// <param name="button">Botón del ratón.</param>
		/// <returns>Estado.</returns>
		ButtonState GetButtonState(ButtonCode button) const;

		/// <summary>
		/// Obtiene si un botón está siendo pulsado.
		/// </summary>
		/// <param name="button">Botón del ratón.</param>
		/// <returns>Estado.</returns>
		bool IsButtonDown(ButtonCode button) const;
		
		//Obtiene si un botón no está siendo pulsado

		/// <summary>
		/// Obtiene si un botón no está siendo pulsado.
		/// </summary>
		/// <param name="button">Botón del ratón.</param>
		/// <returns>Estado.</returns>
		bool IsButtonUp(ButtonCode button) const;

		/// <summary>
		/// Obtiene un rectángulo alrededor del cursor.
		/// </summary>
		/// <param name="size">Ancho y alto del rectángulo.</param>
		/// <returns>Rectángulo.</returns>
		Vector4 GetMouseRectangle(float size = 2.0f) const;

	private:

		/// <summary>
		/// Posición en X, en píxeles, respecto a la esquina superior izquierda.
		/// </summary>
		mouseVar_t positionX;

		/// <summary>
		/// Posición en Y, en píxeles, respecto a la esquina superior izquierda.
		/// </summary>
		mouseVar_t positionY;

		/// <summary>
		/// Posición en X, en porcentaje, reespecto a la esquina superior izquierda.
		/// Entre 0.0 y 1.0.
		/// 0.0 = borde izquierdo.
		/// 1.0 = borde derecho.
		/// </summary>
		mouseVar_t relativePositionX;

		/// <summary>
		/// Posición en Y, en porcentaje, reespecto a la esquina superior izquierda.
		/// Entre 0.0 y 1.0.
		/// 0.0 = borde izquierdo.
		/// 1.0 = borde derecho.
		/// </summary>
		mouseVar_t relativePositionY;

		/// <summary>
		/// Rueda del ratón horizontal.
		/// </summary>
		float_t scrollX = 0.0f;

		/// <summary>
		/// Rueda del ratón vertical (la normal).
		/// </summary>
		float_t scrollY = 0.0f;

		/// <summary>
		/// Rueda del ratón horizontal en el anterior frame.
		/// </summary>
		float_t oldScrollX = 0.0f;

		/// <summary>
		/// Rueda del ratón vertical (la normal) en el anterior frame.
		/// </summary>
		float_t oldScrollY = 0.0f;

		/// <summary>
		/// Estado de los botones. 
		/// 'true' = pressed. 
		/// 'false' = released.
		/// </summary>
		bool buttonStates[NUMBER_OF_BUTTONS];

	};

}
