#pragma once

#include "Event.h"
#include "IEventContainer.h"
#include "DynamicArray.hpp"

namespace OSK::ECS {

	/// @brief Contiene la cola de eventos de un tipo en específico.
	/// @tparam TEvent Tipo de evento.
	/// 
	/// @pre TEvent debe cumplir IsEcsEvent<TEvent>.
	template <typename TEvent> requires IsEcsEvent<TEvent>
	class EventContainer final : public IEventContainer {

	public:

		void _ClearQueue() override {
			eventQueue.Empty();
		}

		EventQueueSpan GetEventQueueSpan() const override {
			EventQueueSpan output{};
			output.data = eventQueue.GetData();
			output.numEntries = eventQueue.GetSize();
			output.eventSize = sizeof(TEvent);

			return output;
		}

		/// @brief Registra un evento en la cola.
		/// @param event Nuevo evento.
		void PublishEvent(const TEvent& event) {
			eventQueue.Insert(event);
		}

		/// @brief Obtiene la cola con todos los eventos del frame actual.
		/// @return Cola con los eventos del frame actual.
		const DynamicArray<TEvent>& GetEventQueue() const {
			return eventQueue;
		}

	private:

		/// @brief Cola con los eventos registrados durante
		/// un frame.
		DynamicArray<TEvent> eventQueue;

	};

}
