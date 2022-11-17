// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "KeyboardState.h"
#include "IQueryInterface.h"

namespace OSK::IO {

	/// @brief Interfaz para leer la entrada del usuario a través
	/// del teclado.
	class OSKAPI_CALL IKeyboardInput {

	public:

		OSK_DEFINE_IUUID((TInterfaceUuid)IUUID::IKeyboardInput);

		virtual ~IKeyboardInput() = default;


		/// @brief Actualiza los estados internos del teclado.
		void UpdateKeyboardInput();


		/// @return True si la tecla está siendo pulsada en este frame.
		bool IsKeyDown(Key key) const;

		/// @return True si la tecla no está siendo pulsada en este frame.
		bool IsKeyUp(Key key) const;


		/// @return Devuelve true si la tecla ha comenzado a pulsarse
		/// en el fotograma actual.
		bool IsKeyStroked(Key key) const;

		/// @return Devuelve true si la tecla ha dejado de pulsarse
		/// en el fotograma actual.
		bool IsKeyReleased(Key key) const;


		/// @return Estado del teclado en el frame actual.
		const KeyboardState& GetKeyboardState() const;

		/// @return Estado del teclado en el frame anterior.
		const KeyboardState& GetPreviousKeyboardState() const;

	protected:

		virtual void UpdateKeyboardState(KeyboardState* currentState) = 0;

	private:

		KeyboardState oldState{};
		KeyboardState newState{};

	};

}
