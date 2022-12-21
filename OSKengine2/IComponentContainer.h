#pragma once

#include "OSKmacros.h"
#include "GameObject.h"

namespace OSK::ECS {

	/// @brief Interfaz común para todos los contenedores de componentes.
	class OSKAPI_CALL IComponentContainer {

	public:

		virtual ~IComponentContainer() = default;

		/// @brief Se llama cuando un objeto es destruido.
		/// Se encarga de eliminar todos los componentes del objeto.
		/// @param obj ID del objeto destruido.
		virtual void GameObjectDestroyerd(GameObjectIndex obj) = 0;

	};

}
