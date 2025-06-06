// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ApiCall.h"
#include "DefineIuuid.h"

namespace OSK {

	/// @brief Identificador �nico para una interfaz.
	enum class IUUID : TInterfaceUuid {
		IGamepadInput = 1,
		IKeyboardInput,
		IMouseInput,
		IFullscreenableDisplay
	};


	/// @brief Esta interfaz permite obtener acceso a los m�todos de
	/// una interfaz en concreto del objeto.
	/// Se usa cuando una clase implementa varias interfaces.
	class OSKAPI_CALL IQueryInterface {

	public:

		virtual ~IQueryInterface() = default;

		/// @brief Permite obtener un puntero para la interfaz dada.
		/// @param interfaceUuid UUID de la interfaz.
		/// @param ptr Puntero a sobreescribir. Ser� nullptr si la clase
		/// no implementa la interfad indicada.
		virtual void QueryInterface(TInterfaceUuid interfaceUuid, void** ptr) const = 0;

		/// @brief Permite obtener un puntero para la interfaz dada.
		/// @param uuid UUID de la interfaz.
		/// @param ptr Puntero a sobreescribir. Ser� nullptr si la clase
		/// no implementa la interfad indicada.
		inline void QueryInterface(IUUID uuid, void** ptr) const {
			QueryInterface((TInterfaceUuid)uuid, ptr);
		}

		virtual void QueryConstInterface(TInterfaceUuid uuid, const void** ptr) const = 0;

		inline void QueryConstInterface(IUUID uuid, const void** ptr) const {
			QueryConstInterface((TInterfaceUuid)uuid, ptr);
		}

	};

}
