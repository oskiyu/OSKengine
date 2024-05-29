#pragma once

#include "ApiCall.h"

#include "Vector2.hpp"

namespace OSK::IO {

	/// <summary>
	/// Botones del ratón.
	/// </summary>
	enum class MouseButton {

		/// <summary>
		/// Botón extra #1.
		/// </summary>
		BUTTON_1,

		/// <summary>
		/// Botón extra #2.
		/// </summary>
		BUTTON_2,

		/// <summary>
		/// Botón extra #3.
		/// </summary>
		BUTTON_3,

		/// <summary>
		/// Botón extra #4.
		/// </summary>
		BUTTON_4,

		/// <summary>
		/// Botón extra #5.
		/// </summary>
		BUTTON_5,

		/// <summary>
		/// Botón extra #6.
		/// </summary>
		BUTTON_6,

		/// <summary>
		/// Botón extra #7.
		/// </summary>
		BUTTON_7,

		/// <summary>
		/// Botón izquierdo.
		/// </summary>
		BUTTON_LEFT,

		/// <summary>
		/// Botón derecho.
		/// </summary>
		BUTTON_RIGHT,

		/// <summary>
		/// Botón de la rueda.
		/// </summary>
		BUTTON_MIDDLE,

		__END

	};

	/// <summary>
	/// Estados en el que puede estar un botón.
	/// </summary>
	enum class ButtonState {

		/// <summary>
		/// No está siendo pulsado.
		/// </summary>
		RELEASED,

		/// <summary>
		/// Está siendo pulsado.
		/// </summary>
		PRESSED

	};

	/// <summary>
	/// Número de teclas disponibles en el teclado.
	/// </summary>
	constexpr int MouseNumberOfButtons = (int)MouseButton::__END;

	/// <summary>
	/// Struct que almacena el estado del ratón en un determinado momento.
	/// Contiene la posición, el scroll y el estado de los botones.
	/// </summary>
	class OSKAPI_CALL MouseState {

	public:

		MouseState();

		/// <summary>
		/// Obtiene el estado de un botón.
		/// </summary>
		/// <param name="button">Botón del ratón.</param>
		/// <returns>Estado.</returns>
		ButtonState GetButtonState(MouseButton button) const;

		/// <summary>
		/// Obtiene si un botón está siendo pulsado.
		/// </summary>
		/// <param name="button">Botón del ratón.</param>
		/// <returns>Estado.</returns>
		bool IsButtonDown(MouseButton button) const;

		/// <summary>
		/// Obtiene si un botón no está siendo pulsado.
		/// </summary>
		/// <param name="button">Botón del ratón.</param>
		/// <returns>Estado.</returns>
		bool IsButtonUp(MouseButton button) const;

		/// <summary>
		/// Posición en píxeles respecto a la esquina superior izquierda de la ventana.
		/// </summary>
		Vector2i GetPosition() const;

		/// <summary>
		/// Posición en porcentaje respecto a la esquina superior izquierda de la ventana.
		/// Entre 0.0 y 1.0.
		/// 0.0 = borde izquierdo.
		/// 1.0 = borde derecho.
		/// </summary>
		Vector2f GetRelativePosition() const;

		/// <summary>
		/// Rueda del ratón vertical (la normal).
		/// </summary>
		int GetScroll() const;

		/// <summary>
		/// Rueda del ratón horizontal.
		/// </summary>
		int GetHorizontalScroll() const;

		void _SetScrollX(int x);
		void _SetScrollY(int y);
		void _SetPosition(const Vector2i& pos);
		void _SetRelativePosition(const Vector2f& pos);
		void _SetButtonState(MouseButton button, ButtonState state);

	private:

		Vector2i position = Vector2i::Zero;
		Vector2f relativePosition = Vector2f::Zero;
		Vector2i scroll = Vector2i::Zero;

		ButtonState buttonStates[MouseNumberOfButtons];

	};

}
