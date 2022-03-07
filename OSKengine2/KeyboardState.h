#pragma once

#include "OSKmacros.h"

namespace OSK::IO {

	/// <summary>
	/// Enum con todas las teclas del teclado disponibles.
	/// </summary>
	enum class Key {
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
	enum class KeyState {

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
	constexpr int KeyboardNumberOfKeys = (int)Key::__END;

	/// <summary>
	/// Representa el estado del teclado en un instante determinado.
	/// Contiene el estado (presionado o suelto) de las teclas.
	/// </summary>
	class OSKAPI_CALL KeyboardState {

	public:

		/// <summary>
		/// Retorna el estado de una tecla dada.
		/// </summary>
		/// <returns>RELEASED o PRESSED.</returns>
		KeyState GetKeyState(Key key) const;

		/// <summary>
		/// Comprueba si una tecla está siendo pulsada.
		/// </summary>
		/// <returns>'true' = PRESSED. 'false' = RELEASED.</returns>
		bool IsKeyDown(Key key) const;

		/// <summary>
		/// Comprueba si una tecla no está siendo pulsada.
		/// </summary>
		/// <returns>'true' = RELEASED. 'false' = PRESSED.</returns>
		bool IsKeyUp(Key key) const;

		/// <summary>
		/// Establece el valor que va a tener la tecla,
		/// esté presionada o no.
		/// 
		/// Para llamarse desde la ventana.
		/// </summary>
		void _SetKeyState(Key key, KeyState state);

	private:

		/// <summary>
		/// Almacena los estados de todas las teclas.
		/// </summary>
		int keyStates[KeyboardNumberOfKeys];

	};

}
