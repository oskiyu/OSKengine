#pragma once

#include <KeyboardState.h>

#include "Wrapper.h"

namespace OSKcs {

	/// <summary>
	/// Enum con todas las teclas del teclado disponibles.
	/// </summary>
	public enum class Key {
		SPACE,
		APOSTROPHE,
		COMMA,
		MINUS,
		PERIDO,
		SLASH,
		KEY_0,
		KEY_1,
		KEY_2,
		KEY_3,
		KEY_4,
		KEY_5,
		KEY_6,
		KEY_7,
		KEY_8,
		KEY_9,
		SEMICOLON,
		EQUAL,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		LEFT_BRACKET,
		BACKSLASH,
		REIGHT_BRACKET,
		GRAVE_ACCENT,
		WORLD_1,
		WORLD_2,
		ESCAPE,
		ENTER,
		TAB,
		BACKSPACE,
		INSERT,
		DEL,
		RIGHT,
		LEFT,
		DOWN,
		UP,
		PAGE_UP,
		PAGE_DOWN,
		HOME,
		END,
		CAPS_LOCK,
		SCROLL_LOCK,
		NUM_LOCK,
		PRINT_SCREEN,
		PAUSE,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		F25,
		KEYPAD_0,
		KEYPAD_1,
		KEYPAD_2,
		KEYPAD_3,
		KEYPAD_4,
		KEYPAD_5,
		KEYPAD_6,
		KEYPAD_7,
		KEYPAD_8,
		KEYPAD_9,
		KEYPAD_DECIMAL,
		KEYPAD_DIVIDE,
		KEYPAD_MULTIPLY,
		KEYPAD_SUBSTRACT,
		KEYPAD_ADD,
		KEYPAD_ENTER,
		KEYPAD_EQUAL,
		LEFT_SHIFT,
		LEFT_CONTROL,
		LEFT_ALT,
		LEFT_SUPER,
		RIGHT_SHIFT,
		RIGHT_CONTROL,
		RIGHT_ALT,
		RIGHT_SUPER,
		MENU,

		ANY_SHIFT,
		ANY_ALT,

		__END
	};

	/// <summary>
	/// Posibles estados de una tecla del teclado.
	/// </summary>
	public enum class KeyState {

		/// <summary>
		/// No está siendo pulsado.
		/// </summary>
		RELEASED,

		/// <summary>
		/// Está siendo pulsado.
		/// </summary>
		PRESSED

	};

	public ref class KeyboardState : public Wrapper<OSK::KeyboardState> {

	public:

		KeyboardState() {
		
		}

		const static int NumberOfKeys = (int)OSK::Key::__END;

		KeyState GetKeyState(Key key) {
			return (KeyState)instance->GetKeyState((OSK::Key)key);
		}

		bool IsKeyDown(Key key) {
			return instance->IsKeyDown((OSK::Key)key);
		}

		bool IsKeyUp(Key key) {
			return instance->IsKeyUp((OSK::Key)key);
		}

	};

}
