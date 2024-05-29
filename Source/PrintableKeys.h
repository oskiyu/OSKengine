#pragma once

#include "KeyboardState.h"

#include <string>


namespace OSK::IO {

	/// @brief Array constexpr que contiene todas las
	/// teclas que se corresponden con caracteres
	/// que se pueden imprimir.
	static constexpr Key PrintableKeys[] = {
		Key::SPACE,
		Key::COMMA,
		Key::MINUS,
		Key::PERIOD,
		Key::SLASH,
		Key::KEY_0,
		Key::KEY_1,
		Key::KEY_2,
		Key::KEY_3,
		Key::KEY_4,
		Key::KEY_5,
		Key::KEY_6,
		Key::KEY_7,
		Key::KEY_8,
		Key::KEY_9,
		Key::SEMICOLON,
		Key::EQUAL,
		Key::A,
		Key::B,
		Key::C,
		Key::D,
		Key::E,
		Key::F,
		Key::G,
		Key::H,
		Key::I,
		Key::J,
		Key::K,
		Key::L,
		Key::M,
		Key::N,
		Key::O,
		Key::P,
		Key::Q,
		Key::R,
		Key::S,
		Key::T,
		Key::U,
		Key::V,
		Key::W,
		Key::x,
		Key::y,
		Key::Z,
		Key::LEFT_BRACKET,
		Key::RIGHT_BRACKET,
		Key::BACKSLASH,
		Key::TAB,
		Key::KEYPAD_0,
		Key::KEYPAD_1,
		Key::KEYPAD_2,
		Key::KEYPAD_3,
		Key::KEYPAD_4,
		Key::KEYPAD_5,
		Key::KEYPAD_6,
		Key::KEYPAD_7,
		Key::KEYPAD_8,
		Key::KEYPAD_9
	};


	/// @param key Tecla del teclado.
	/// @param isMayus True si debe ser mayúsculas
	/// (pulsando SHIFT, por ejemplo).
	/// @return Carácter que se corresponde con 
	/// la tecla indicada.
	/// 
	/// @pre @p key debe estar dentro de PrintableKeys. 
	constexpr char GetPrintableKeyText(const Key& key, bool isMayus) {
		switch (key) {
			case Key::SPACE: return ' ';
			case Key::COMMA: return ',';
			case Key::MINUS: return '-';
			case Key::PERIOD: return '.';
			case Key::SLASH: return '/';
			case Key::KEY_0: return '0';
			case Key::KEY_1: return '1';
			case Key::KEY_2: return '2';
			case Key::KEY_3: return '3';
			case Key::KEY_4: return '4';
			case Key::KEY_5: return '5';
			case Key::KEY_6: return '6';
			case Key::KEY_7: return '7';
			case Key::KEY_8: return '8';
			case Key::KEY_9: return '9';
			case Key::SEMICOLON: return ';';
			case Key::EQUAL: return '=';
			case Key::A: return isMayus ? 'A' : 'a';
			case Key::B: return isMayus ? 'B' : 'b';
			case Key::C: return isMayus ? 'C' : 'c';
			case Key::D: return isMayus ? 'D' : 'd';
			case Key::E: return isMayus ? 'E' : 'e';
			case Key::F: return isMayus ? 'F' : 'f';
			case Key::G: return isMayus ? 'G' : 'g';
			case Key::H: return isMayus ? 'H' : 'h';
			case Key::I: return isMayus ? 'I' : 'i';
			case Key::J: return isMayus ? 'J' : 'j';
			case Key::K: return isMayus ? 'K' : 'k';
			case Key::L: return isMayus ? 'L' : 'l';
			case Key::M: return isMayus ? 'M' : 'm';
			case Key::N: return isMayus ? 'N' : 'n';
			case Key::O: return isMayus ? 'O' : 'o';
			case Key::P: return isMayus ? 'P' : 'p';
			case Key::Q: return isMayus ? 'Q' : 'q';
			case Key::R: return isMayus ? 'R' : 'r';
			case Key::S: return isMayus ? 'S' : 's';
			case Key::T: return isMayus ? 'T' : 't';
			case Key::U: return isMayus ? 'U' : 'u';
			case Key::V: return isMayus ? 'V' : 'v';
			case Key::W: return isMayus ? 'W' : 'w';
			case Key::x: return isMayus ? 'X' : 'x';
			case Key::y: return isMayus ? 'Y' : 'y';
			case Key::Z: return isMayus ? 'Z' : 'z';
			case Key::LEFT_BRACKET: return '{';
			case Key::RIGHT_BRACKET: return '}';
			case Key::BACKSLASH: return '\\';
			case Key::TAB: return '\t';
			case Key::KEYPAD_0: return '0';
			case Key::KEYPAD_1: return '1';
			case Key::KEYPAD_2: return '2';
			case Key::KEYPAD_3: return '3';
			case Key::KEYPAD_4: return '4';
			case Key::KEYPAD_5: return '5';
			case Key::KEYPAD_6: return '6';
			case Key::KEYPAD_7: return '7';
			case Key::KEYPAD_8: return '8';
			case Key::KEYPAD_9: return '9';
			default: return ' ';
		}
	}
}
