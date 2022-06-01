#pragma once

#include "OSKmacros.h"

#include <GLFW/glfw3.h>

namespace OSK::IO {

	/// <summary>
	/// Todos y cada uno de los ejes del gamepad,
	/// es decir, cada uno de los inputs que pueden
	/// tener un valor entre 0.0 y 1.0 ó -1.0 y 1.0
	/// </summary>
	enum class GamepadAxis {

		/// <summary>
		/// Eje horizontal del joystick izquierdo.
		/// </summary>
		/// 
		/// @note Derecha: 1.0
		/// @note Izquierda: -1.0
		LEFT_X = GLFW_GAMEPAD_AXIS_LEFT_X,

		/// <summary>
		/// Eje vertical del joystick izquierdo.
		/// </summary>
		/// 
		/// @note Arriba: -1.0
		/// @note Abajo: 1.0
		LEFT_Y = GLFW_GAMEPAD_AXIS_LEFT_Y,

		/// <summary>
		/// Eje horizontal del joystick derecho.
		/// </summary>
		/// 
		/// @note Derecha: 1.0
		/// @note Izquierda: -1.0
		RIGHT_X = GLFW_GAMEPAD_AXIS_RIGHT_X,

		/// <summary>
		/// Eje horizontal del joystick derecho.
		/// </summary>
		/// 
		/// @note Arriba: -1.0
		/// @note Abajo: 1.0
		RIGHT_Y = GLFW_GAMEPAD_AXIS_RIGHT_Y,

		/// <summary>
		/// Gatillo izquierdo.
		/// </summary>
		/// 
		/// @note Entre 0.0 y 1.0
		L2 = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,

		/// <summary>
		/// Gatillo derecho.
		/// </summary>
		/// 
		/// @note Entre 0.0 y 1.0
		R2 = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,

		__END

	};

	/// <summary>
	/// Botones del gamepad.
	/// Pueden estar pulsados o no, no hay valores intermedios.
	/// </summary>
	/// 
	/// @note Sigue la notación de un mando de XBox, pero puede ser
	/// usado con cualquier mando compatible con la plataforma de destino.
	enum class GamepadButton {

		X = GLFW_GAMEPAD_BUTTON_X,
		Y = GLFW_GAMEPAD_BUTTON_Y,
		A = GLFW_GAMEPAD_BUTTON_A,
		B = GLFW_GAMEPAD_BUTTON_B,

		START = GLFW_GAMEPAD_BUTTON_START,
		BACK = GLFW_GAMEPAD_BUTTON_BACK,

		L1 = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
		R1 = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,

		PAD_TOP = GLFW_GAMEPAD_BUTTON_DPAD_UP,
		PAD_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
		PAD_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
		PAD_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT

	};


	/// <summary>
	/// Estados en el que puede estar un botón del gamepad.
	/// </summary>
	enum class GamepadButtonState {
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
	/// Representa el estado actual de un gamepad conectado a la 
	/// plataforma.
	/// </summary>
	/// 
	/// @note Puede no estar conectado, se debe comprobar que esté conectado.
	/// 
	/// @warning En caso de no estar conectado, los valores devueltos pueden
	/// ser incoherentes. Comprobar siempre que el gamepad está conectado.
	class OSKAPI_CALL GamepadState {

	public:

		GamepadState() { }
		GamepadState(TSize identifier);

		GamepadButtonState GetButtonState(GamepadButton button) const;

		bool IsButtonDown(GamepadButton button) const;
		bool IsButtonUp(GamepadButton button) const;

		/// <summary>
		/// Devuelve el estado del eje dado.
		/// 
		/// Véase OSK::IO::GamepadAxis.
		/// </summary>
		/// 
		/// @note Los valores mínimo y máximo pueden ser 0 a 1 ó -1 a 1.
		/// Véase OSK::IO::GamepadAxis.
		float GetAxisState(GamepadAxis axis) const;

		/// <summary>
		/// Devuelve el identificador del mando.
		/// 
		/// Entre 0-3.
		/// </summary>
		/// 
		/// @note El mando principal tendrá el identificador 0.
		TSize GetIdentifier() const;

		/// <summary>
		/// Comrpueba si el mando está conectado. SI no o etá
		/// los valores devueltos por las otras funciones serán inválidos.
		/// </summary>
		bool IsConnected() const;

		void _SetButtonState(GamepadButton button, GamepadButtonState state);
		void _SetAxisState(GamepadAxis axis, float value);
		void _SetConnectionState(bool isConnected);

	private:

		GamepadButtonState buttonState[4]{};
		float axesStates[6]{};

		TSize identifier = 0;

		bool isConnected = false;

	};

}
