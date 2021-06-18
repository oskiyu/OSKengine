#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "GameObjectManager.h"

namespace OSK::ECS {

	/// <summary>
	/// Clase base de un array de componentes.
	/// </summary>
	class OSKAPI_CALL IComponentArray {

	public:

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~IComponentArray() = default;

		/// <summary>
		/// Quita los componentes del objeto que ha sido eliminado.
		/// </summary>
		/// <param name="obj">ID del objeto eliminado.</param>
		virtual void GameObjectDestroyed(GameObjectID obj) = 0;

	};

}