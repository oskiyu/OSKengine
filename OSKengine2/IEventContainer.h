#pragma once

#include "OSKmacros.h"

namespace OSK::ECS {

	/// @brief Interfaz común para todos los contenedores de eventos.
	class OSKAPI_CALL IEventContainer {

	public:

		virtual ~IEventContainer() = default;

		/// @brief Elimina todos los eventos de la cola.
		/// Debe ser llamado al final de cada frame.
		virtual void _ClearQueue() = 0;

	};

}
