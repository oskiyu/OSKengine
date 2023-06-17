#pragma once

#include "Event.h"
#include "IEventContainer.h"
#include "EventContainer.h"
#include "HashMap.hpp"
#include "UniquePtr.hpp"

#include "EventNotRegisteredException.h"

#include <format>

namespace OSK::ECS {

	/// @brief Se encarga de gestionar las colas de eventos.
	class OSKAPI_CALL EventManager final {

	public:

		/// @brief Registra un tipo de evento.
		/// Los venetos deben registrarse así para poder ser manejados
		/// por ECS.
		/// @tparam TEvent Tipo de evento.
		/// 
		/// @pre TEvent debe cumplir IsEcsEvent<TEvent>.
		template <typename TEvent>  requires IsEcsEvent<TEvent>
		void RegisterEventType() {
			const auto key = static_cast<std::string>(TEvent::GetEventName());

			containers.Insert(key, new EventContainer<TEvent>());
		}


		/// @brief Publica un evento, de tal manera que puede ser
		/// procesado por los sistemas consumidores.
		/// @tparam TEvent Tipo de evento.
		/// @param event Evento.
		/// 
		/// @pre TEvent debe cumplir IsEcsEvent<TEvent>.
		/// @pre TEvent debe haber sido registrado con RegisterEventType.
		template <typename TEvent> requires IsEcsEvent<TEvent>
		void PublishEvent(const TEvent& event) {
			const auto name = static_cast<std::string>(TEvent::GetEventName());
			auto& container = static_cast<EventContainer<TEvent>&>(containers.Get(name).GetValue());

			container.PublishEvent(event);
		}

		/// @brief Obtiene la cola con todos los eventos de un tipo
		/// que se han producido en un frame.
		/// @tparam TEvent Tipo de evento.
		/// @return Cola con todos los eventos. Puede ser una cola vacía si
		/// no se ha producido ningún evento en el frame.
		/// 
		/// @pre TEvent debe cumplir IsEcsEvent<TEvent>.
		/// @pre TEvent debe haber sido registrado con RegisterEventType.
		/// 
		/// @throws EventNotRegisteredException Si se trata de obtener una cola a un tipo de evento
		/// que no ha sido previamente registrado.
		template <typename TEvent> requires IsEcsEvent<TEvent>
		const DynamicArray<TEvent>& GetEventQueue() const {
			const auto name = static_cast<std::string>(TEvent::GetEventName());

			if (!containers.ContainsKey(name))
				throw EventNotRegisteredException(TEvent::GetEventName());

			const auto& container = static_cast<const EventContainer<TEvent>&>(
				containers.Get(name).GetValue());

			return container.GetEventQueue();
		}

		/// @brief Elimina los contenidos de todas las colas.
		/// Debe llamarse al final de cada frame.
		void _ClearQueues() {
			for (const auto& [name, queue] : containers)
				queue->_ClearQueue();
		}

		/// @tparam TEvent Tipo del evento.
		/// @return True si el evento fue registrado.
		template <typename TEvent> requires IsEcsEvent<TEvent>
		bool EventHasBeenRegistered() const {
			return containers.ContainsKey((std::string)TEvent::GetEventName());
		}

	private:

		/// @brief Contenedores.
		HashMap<std::string, UniquePtr<IEventContainer>> containers;

	};

}
