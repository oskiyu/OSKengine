// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "IUserInput.h"

#include "IKeyboardInput.h"
#include "IMouseInput.h"
#include "IGamepadInput.h"

namespace OSK::IO {

	class Window;

	/// @brief Input para PC (Windows / Linux / Mac).
	/// 
	/// Implementa las interfaces:
	/// - IKeyboardInput.
	/// - IMouseInput.
	/// - IGamepadInput.
	/// 
	/// @note Se debe llamar a PcUserInput::Initialize.
	class OSKAPI_CALL PcUserInput : public IUserInput,
		public IKeyboardInput, public IMouseInput, public IGamepadInput {

	public:

		/// @brief Establece los callbacks para el manejo del ratón.
		/// @param window Ventana del juego.
		void Initialize(const Window& window);

		void Update() override;

		void QueryInterface(TInterfaceUuid uuid, void** ptr) const override final;
		void QueryConstInterface(TInterfaceUuid uuid, const void** ptr) const override;

	private:

		void SetupMotionMode() override;
		void SetupReturnMode() override;

		void SetMousePosition(const Vector2ui& pos) override;

		void UpdateKeyboardState(KeyboardState* state) override;
		void UpdateMouseState(MouseState* state) override;
		void UpdateGamepadState(GamepadState* state) override;

		/// @brief Callback para el movimiento del ratón.
		/// @param window  Ventana GLFW.
		/// @param posX Posición en X, en píxeles.
		/// @param posY Posición en Y, en píxeles.
		static void MouseInputCallback(struct GLFWwindow* window, double posX, double posY);

		/// @brief Callback para el movimiento de la rueda del ratón.
		/// @param window Ventana GLFW.
		/// @param dX Movimiento horizontal (no en todos los ratones).
		/// @param dY Movimiento vertical.
		static void MouseScrollCallback(struct GLFWwindow* window, double dX, double dY);

		/// @brief En este estado se guardará la información
		/// obtenida a través de los callbacks.
		MouseState mouseCallbackState{};


		static PcUserInput* self;

	};

}
