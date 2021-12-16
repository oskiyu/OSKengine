#pragma once

#include <MouseState.h>

#include "Wrapper.h"

namespace OSKcs {

	/// <summary>
	/// Botones del rat�n.
	/// </summary>
	public enum class MouseButton {

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
	public enum class ButtonState {

		/// <summary>
		/// No est� siendo pulsado.
		/// </summary>
		RELEASED,

		/// <summary>
		/// Est� siendo pulsado.
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
