#pragma once

#include "OSKmacros.h"
#include "GameObject.h"

namespace OSK::ECS {

	/// <summary>
	/// Clase base para el contenedor de componentes.
	/// </summary>
	class OSKAPI_CALL IComponentContainer {

	public:

		virtual ~IComponentContainer() = default;

		/// <summary>
		/// Se llama cuando un objeto es destruido.
		/// Se encarga de eliminar todos los componentes del objeto.
		/// </summary>
		virtual void GameObjectDestroyerd(GameObjectIndex obj) = 0;

	};

}
