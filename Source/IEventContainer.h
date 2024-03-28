#pragma once

#include "OSKmacros.h"

namespace OSK::ECS {

	/// @brief Indica un subrango de una lista de eventos.
	struct EventQueueSpan {

		/// @brief Direcci�n de memoria del primer evento.
		const void* data;

		/// @brief N�mero de eventos.
		USize64 numEntries;
	};


	/// @brief Interfaz com�n para todos los contenedores de eventos.
	class OSKAPI_CALL IEventContainer {

	public:

		virtual ~IEventContainer() = default;

		/// @brief Elimina todos los eventos de la cola.
		/// Debe ser llamado al final de cada frame.
		virtual void _ClearQueue() = 0;

		/// @return EventQueueSpan con todos los eventos de esta cola.
		virtual EventQueueSpan GetEventQueueSpan() const = 0;

	};

}
