#pragma once

#include "ApiCall.h"
#include "NumericTypes.h"
#include "Vector2.hpp"

namespace OSK::IO {

	enum class Key;
	enum class MouseButton;
	enum class GamepadButton;
	enum class GamepadAxis;

	/// <summary>
	/// Clase que reacciona a los eventos de entrada.
	/// 
	/// Se pueden crear clases derivadas que implementen la
	/// funcionalidad deseada para cada caso.
	/// 
	/// Si las instancias de las clases derivadas se introducen
	/// en Engine::GetInputManager(), se manejar�n
	/// de manera autom�tica una vez por frame.
	/// </summary>
	class OSKAPI_CALL IInputListener {

	public:

		/// <summary>
		/// Es llamada cuando una tecla comienza a estar presionada.
		/// 
		/// PreviousState = RELEASED && CurrentState = PRESSED.
		/// </summary>
		virtual void OnKeyStroke(Key key) { }

		/// <summary>
		/// Es llamada cuando una tecla est� siendo presionada.
		/// 
		/// CurrentState = PRESSED.
		/// </summary>
		virtual void OnKeyBeingPressed(Key key) { }

		/// <summary>
		/// Es llamada cuando una tecla deja de estar presionada.
		/// 
		/// PreviousState = PRESSED && CurrentState = RELEASED.
		/// </summary>
		virtual void OnKeyReleased(Key key) { }

		/// <summary>
		/// Es llamada cuando un bot�n del rat�n comienza a estar presionado.
		/// 
		/// PreviousState = RELEASED && CurrentState = PRESSED.
		/// </summary>
		virtual void OnMouseButtonStroke(MouseButton button) { }

		/// <summary>
		/// Es llamada cuando un bot�n del rat�n est� siendo presionado.
		/// 
		/// CurrentState = PRESSED.
		/// </summary>
		virtual void OnMouseButtonBeingPressed(MouseButton button) { }

		/// <summary>
		/// Es llamada cuando un bot�n del rat�n deja de estar presionado.
		/// 
		/// PreviousState = PRESSED && CurrentState = RELEASED.
		/// </summary>
		virtual void OnMouseButtonReleased(MouseButton button) { }


		/// <summary>
		/// Llamado cuando el rat�n se mueve.
		/// </summary>
		/// <param name="diff">Cantidad de p�xeles que se ha movido en la pantalla.</param>
		virtual void OnMouseMoved(const Vector2i& diff) { }

		/// <summary>
		/// Llamado cuando la rueda del rat�n se mueve.
		/// </summary>
		/// <param name="diff">
		///		X: diferencia horizontal.
		///		Y: diferencia vertical (la rueda normal).
		/// </param>
		virtual void OnMouseWheelMoved(const Vector2i& diff) { }

		/// <summary>
		/// Es llamada cuando un bot�n del mando comienza a estar presionado.
		/// 
		/// PreviousState = RELEASED && CurrentState = PRESSED.
		/// </summary>
		/// <param name="gamepadId">Id del mando (0 - 3)</param>
		virtual void OnGamepadButtonStroke(GamepadButton button, UIndex32 gamepadId) { }

		/// <summary>
		/// Es llamada cuando un bot�n del mando est� siendo presionado.
		/// 
		/// CurrentState = PRESSED.
		/// </summary>
		/// <param name="gamepadId">Id del mando (0 - 3)</param>
		virtual void OnGamepadButtonBeingPressed(GamepadButton button, UIndex32 gamepadId) { }

		/// <summary>
		/// Es llamada cuando un bot�n del mando deja de estar presionado.
		/// 
		/// PreviousState = PRESSED && CurrentState = RELEASED.
		/// </summary>
		/// <param name="gamepadId">Id del mando (0 - 3)</param>
		virtual void OnGamepadButtonReleased(GamepadButton button, UIndex32 gamepadId) { }

		/// <summary>
		/// Llamado cuando uno de los ejes se manteien con un valor > 0 en valor absoluto.
		/// 
		/// Si pasa a ser 0, se llamar� a esta funci�n una �nica vez con el valor 0.
		/// </summary>
		/// <param name="axis">Eje.</param>
		/// <param name="value">Valor actual.</param>
		/// <param name="gamepadId"></param>
		virtual void OnGamepadAxisValue(GamepadAxis axis, float value, UIndex32 gamepadId) { }

		/// <summary>
		/// Establece el valor m�nimo, en valor absoluto, que debe tener el eje
		/// para que se considere activo.
		/// 
		/// Se usa para evitar problemas de drifting.
		/// </summary>
		/// 
		/// @note Debe estar entre 0 y 1.
		/// @note Si se introduce un valor fuera del rango v�lido, se har� clamp a dicho valor.
		void SetMinAxisValue(float value);

		float GetMinAxisValue() const;

	private:

		float axisMin = 0.15f;

	};

}
