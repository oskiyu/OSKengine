// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "MouseState.h"
#include "MouseModes.h"
#include "IQueryInterface.h"

namespace OSK::IO {

	/// @brief Interfaz para leer la entrada del usuario a trav�s
	/// del rat�n.
	class OSKAPI_CALL IMouseInput {

	public:

		OSK_DEFINE_IUUID((TInterfaceUuid)IUUID::IMouseInput);

		virtual ~IMouseInput() = default;


		/// @brief Actualiza los estados internos del rat�n.
		void UpdateMouseInput();


		/// @return Estado del rat�n en el frame actual.
		const MouseState& GetMouseState() const;

		/// @return Estado del rat�n en el frame anterior.
		const MouseState& GetPreviousMouseState() const;


		/// @return Estado actual de MouseMotionMode.
		inline MouseMotionMode GetMotionMode() const { return motionMode; }

		/// @return Estado actual de MouseReturnMode.
		inline MouseReturnMode GetReturnMode() const { return returnMode; }


		/// @brief Actualiza el MouseMotionMode.
		inline void SetMotionMode(MouseMotionMode mode) { motionMode = mode; SetupMotionMode();  }

		/// @brief Actualiza el MouseReturnMode.
		inline void SetReturnMode(MouseReturnMode mode) { returnMode = mode; SetupReturnMode(); }


		/// @brief Establece la posici�n del rat�n, desde la esquina superior izquierda.
		/// @param posX Posici�n, en pixeles, en el eje horizontal.
		/// @param posY Posici�n, en pixeles, en el eje vertical.
		virtual void SetMousePosition(const Vector2ui& pos) = 0;

	protected:

		/// @brief La implementaci�n debe actualizar de manera
		/// el motion mode obtenido a trav�s de GetMotionMode.
		virtual void SetupMotionMode() = 0;

		/// @brief La implementaci�n debe actualizar de manera
		/// el return mode obtenido a trav�s de GetReturnMode.
		virtual void SetupReturnMode() = 0;

		virtual void UpdateMouseState(MouseState* currentState) = 0;

	private:

		MouseMotionMode motionMode = MouseMotionMode::RAW;
		MouseReturnMode returnMode = MouseReturnMode::FREE;

		MouseState oldState{};
		MouseState newState{};

	};

}
