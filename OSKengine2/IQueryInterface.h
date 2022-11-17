// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// @brief Identificador único para una interfaz.
	enum class IUUID : TInterfaceUuid {
		IGamepadInput = 1,
		IKeyboardInput,
		IMouseInput,
		IFullscreenableDisplay
	};


	/// @brief Esta interfaz permite obtener acceso a los métodos de
	/// una interfaz en concreto del objeto.
	/// Se usa cuando una clase implementa varias interfaces.
	class OSKAPI_CALL IQueryInterface {

	public:

		virtual ~IQueryInterface() = default;

		/// @brief Permite obtener un puntero para la interfaz dada.
		/// @param interfaceUuid UUID de la interfaz.
		/// @param ptr Puntero a sobreescribir. Será nullptr si la clase
		/// no implementa la interfad indicada.
		virtual void QueryInterface(TInterfaceUuid interfaceUuid, void** ptr) const = 0;

		/// @brief Permite obtener un puntero para la interfaz dada.
		/// @param interfaceUuid UUID de la interfaz.
		/// @param ptr Puntero a sobreescribir. Será nullptr si la clase
		/// no implementa la interfad indicada.
		inline void QueryInterface(IUUID uuid, void** ptr) const {
			QueryInterface((TInterfaceUuid)uuid, ptr);
		}

	};

}
