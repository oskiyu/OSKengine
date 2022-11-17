// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"
#include "IQueryInterface.h"

namespace OSK::IO {

	class IKeyboardInput;
	class IMouseInput;
	class IGamepadInput;

	/// @brief Interfaz para clases que permiten recoger
	/// la entrada del usuario.
	/// 
	/// Toda clase derivada puede también implementar estas otras interfaces:
	/// - IKeyboardInput.
	/// - IMouseInput.
	/// - IGamepadInput.
	class OSKAPI_CALL IUserInput : public IQueryInterface {

	public:

		virtual ~IUserInput() = default;

		/// @brief Actualiza los estados internos del input.
		virtual void Update() = 0;

	};

}
