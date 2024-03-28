#pragma once

#include "ISystem.h"
#include "Event.h"
#include "EventContainer.h"

namespace OSK::ECS {

	/// @brief Un sistema consumidor es un sistema que se encarga de responder a eventos.
	/// Se recomienda no heredar directamente de esta clase, sino de la clase
	/// ITypedConsumerSystem, indicando el tipo de evento al que se reacciona.
	class OSKAPI_CALL IConsumerSystem : public ISystem {

	public:

		~IConsumerSystem() override = default;

		/// @brief Función que se ejecuta antes de comenzar la ejecución del sistema.
		virtual void OnExecutionStart();

		/// @brief Ejecuta la lógica del sistema para un rango de eventos.
		/// Los eventos deben ser casteados dentro de la función.
		/// Para ello, se puede usar la función BuildSpan().
		/// @param deltaTime Tiempo (en segundos) transcurrido desde la ejecución anterior.
		/// @param events Lista de eventos.
		virtual void Execute(TDeltaTime deltaTime, EventQueueSpan events);

		/// @brief Función que se ejecuta después de finalizar la ejecución del sistema.
		virtual void OnExecutionEnd();


		/// @return Nombre del tipo de evento asociado al sistema.
		virtual std::string_view GetEventName() const = 0;

	protected:

		IConsumerSystem() = default;

		/// @brief Construye un span de eventos a partir de la información dada.
		/// @tparam TEvent Tipo de evento.
		/// @param firstEvent Dirección de memoria del primer evento.
		/// @param numEvents Número de eventos.
		/// @return Span de eventos construido.
		template <typename TEvent>
		requires IsEcsEvent<TEvent>
		std::span<const TEvent> BuildSpan(const void* firstEvent, USize64 numEvents) {
			return std::span<const TEvent>(static_cast<const TEvent*>(firstEvent), numEvents);
		}

	};


	/// @brief Un sistema consumidor es un sistema que se encarga de responder a eventos.
	/// @tparam TEvent Tipo de evento asociado al sistema.
	template <typename TEvent>
	requires IsEcsEvent<TEvent>
	class OSKAPI_CALL ITypedConsumerSystem : public IConsumerSystem {

	public:

		~ITypedConsumerSystem() override = default;
 
		void Execute(TDeltaTime deltaTime, EventQueueSpan events) override {
			Execute(deltaTime, BuildSpan<TEvent>(events.data, events.numEntries));
		}

		/// @brief Ejecuta la lógica del sistema para un rango de eventos.
		/// @param deltaTime Tiempo (en segundos) transcurrido desde la ejecución anterior.
		/// @param events Lista de eventos.
		virtual void Execute(TDeltaTime deltaTime, std::span<const TEvent> events) {
			(void)0;
		}

		std::string_view GetEventName() const override {
			return TEvent::GetEventName();
		}

	};

	/// @brief Concepto para saber si un sistema es un sistema consumidor.
	template<typename TSystem>
	concept IsConsumerSystem = std::is_base_of_v<IConsumerSystem, TSystem>
		&& IsEcsSystem<TSystem>;

}
