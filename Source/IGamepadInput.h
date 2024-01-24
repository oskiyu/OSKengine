// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "GamepadState.h"
#include "IQueryInterface.h"

namespace OSK::IO {

	/// @brief Interfaz para leer la entrada del usuario a través
	/// de uno o varios mandos.
	class OSKAPI_CALL IGamepadInput {

	public:

		OSK_DEFINE_IUUID((TInterfaceUuid)IUUID::IGamepadInput)

		/// @brief Número máximo de gamepads que pueden estar conectados.
		const static UIndex32 MAX_GAMEPAD_COUNT = 4;


		IGamepadInput();

		virtual ~IGamepadInput() = default;


		/// @brief Actualiza los estados internos de los gamepads.
		void UpdateGamepadInput();


		/// @return Estado del gamepad en el frame actual.
		/// @pre 0 <= index < IGamepadInput::MAX_GAMEPAD_COUNT.
		const GamepadState& GetGamepadState(UIndex32 index) const;

		/// @return Estado del gamepad en el frame anterior.
		/// @pre 0 <= index < IGamepadInput::MAX_GAMEPAD_COUNT.
		const GamepadState& GetPreviousGamepadState(UIndex32 index) const;

	protected:

		virtual void UpdateGamepadState(GamepadState* currentState) = 0;

	private:

		std::array<GamepadState, MAX_GAMEPAD_COUNT> oldStates{};
		std::array<GamepadState, MAX_GAMEPAD_COUNT> newStates{};

	};

}
