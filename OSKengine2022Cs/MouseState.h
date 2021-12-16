#pragma once

#include <MouseState.h>

#include "Wrapper.h"

namespace OSKcs {

	/// <summary>
	/// Botones del ratón.
	/// </summary>
	public enum class MouseButton {

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
	public enum class ButtonState {

		/// <summary>
		/// No está siendo pulsado.
		/// </summary>
		RELEASED,

		/// <summary>
		/// Está siendo pulsado.
		/// </summary>
		PRESSED

	};


	public ref class MouseState : public Wrapper<OSK::MouseState> {

	public:

		MouseState() {

		}

		const static int NumberOfButtons = (int)OSK::MouseButton::__END;

		ButtonState GetButtonState(MouseButton button) {
			return (ButtonState)instance->GetButtonState((OSK::MouseButton)button);
		}

		bool IsButtonDown(MouseButton button) {
			return instance->IsButtonDown((OSK::MouseButton)button);
		}

		bool IsButtonUp(MouseButton button) {
			return instance->IsButtonUp((OSK::MouseButton)button);
		}

	};

}
