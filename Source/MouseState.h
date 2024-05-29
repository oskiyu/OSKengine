#pragma once

#include "ApiCall.h"

#include "Vector2.hpp"

namespace OSK::IO {

	/// <summary>
	/// Botones del rat�n.
	/// </summary>
	enum class MouseButton {

		/// <summary>
		/// Bot�n extra #1.
		/// </summary>
		BUTTON_1,

		/// <summary>
		/// Bot�n extra #2.
		/// </summary>
		BUTTON_2,

		/// <summary>
		/// Bot�n extra #3.
		/// </summary>
		BUTTON_3,

		/// <summary>
		/// Bot�n extra #4.
		/// </summary>
		BUTTON_4,

		/// <summary>
		/// Bot�n extra #5.
		/// </summary>
		BUTTON_5,

		/// <summary>
		/// Bot�n extra #6.
		/// </summary>
		BUTTON_6,

		/// <summary>
		/// Bot�n extra #7.
		/// </summary>
		BUTTON_7,

		/// <summary>
		/// Bot�n izquierdo.
		/// </summary>
		BUTTON_LEFT,

		/// <summary>
		/// Bot�n derecho.
		/// </summary>
		BUTTON_RIGHT,

		/// <summary>
		/// Bot�n de la rueda.
		/// </summary>
		BUTTON_MIDDLE,

		__END

	};

	/// <summary>
	/// Estados en el que puede estar un bot�n.
	/// </summary>
	enum class ButtonState {

		/// <summary>
		/// No est� siendo pulsado.
		/// </summary>
		RELEASED,

		/// <summary>
		/// Est� siendo pulsado.
		/// </summary>
		PRESSED

	};

	/// <summary>
	/// N�mero de teclas disponibles en el teclado.
	/// </summary>
	constexpr int MouseNumberOfButtons = (int)MouseButton::__END;

	/// <summary>
	/// Struct que almacena el estado del rat�n en un determinado momento.
	/// Contiene la posici�n, el scroll y el estado de los botones.
	/// </summary>
	class OSKAPI_CALL MouseState {

	public:

		MouseState();

		/// <summary>
		/// Obtiene el estado de un bot�n.
		/// </summary>
		/// <param name="button">Bot�n del rat�n.</param>
		/// <returns>Estado.</returns>
		ButtonState GetButtonState(MouseButton button) const;

		/// <summary>
		/// Obtiene si un bot�n est� siendo pulsado.
		/// </summary>
		/// <param name="button">Bot�n del rat�n.</param>
		/// <returns>Estado.</returns>
		bool IsButtonDown(MouseButton button) const;

		/// <summary>
		/// Obtiene si un bot�n no est� siendo pulsado.
		/// </summary>
		/// <param name="button">Bot�n del rat�n.</param>
		/// <returns>Estado.</returns>
		bool IsButtonUp(MouseButton button) const;

		/// <summary>
		/// Posici�n en p�xeles respecto a la esquina superior izquierda de la ventana.
		/// </summary>
		Vector2i GetPosition() const;

		/// <summary>
		/// Posici�n en porcentaje respecto a la esquina superior izquierda de la ventana.
		/// Entre 0.0 y 1.0.
		/// 0.0 = borde izquierdo.
		/// 1.0 = borde derecho.
		/// </summary>
		Vector2f GetRelativePosition() const;

		/// <summary>
		/// Rueda del rat�n vertical (la normal).
		/// </summary>
		int GetScroll() const;

		/// <summary>
		/// Rueda del rat�n horizontal.
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
