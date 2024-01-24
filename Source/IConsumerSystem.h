#pragma once

#include "ISystem.h"
#include "Event.h"

namespace OSK::ECS {

	/// @brief Interfaz común para todos los sistemas consumidores.
	class OSKAPI_CALL IConsumerSystem : public ISystem {

	public:

		~IConsumerSystem() override = default;

	protected:

		IConsumerSystem() = default;

	};

	/// @brief Un sistema consumidor es un sistema que se encarga de responder a eventos.
	/// Estos sistemas se ejecutan después de los sistemas productores y antes que los sistemas puros.
	/// @tparam TEvent Tipo de evento al que está suscrito.
	template <typename TEvent> requires IsEcsEvent<TEvent>
	class OSKAPI_CALL ConsumerSystem : public IConsumerSystem {

	public:

		~ConsumerSystem() override = default;

	protected:

		ConsumerSystem() = default;

	};


	/// @brief Concepto para saber si un sistema es un sistema consumidor.
	template<typename TSystem, typename TEvent>
	concept IsConsumerSystemEvent = std::is_base_of_v<ConsumerSystem<TEvent>, TSystem>
		&& IsEcsSystem<TSystem>;

	/// @brief Concepto para saber si un sistema es un sistema consumidor.
	template<typename TSystem>
	concept IsConsumerSystem = std::is_base_of_v<IConsumerSystem, TSystem>
		&& IsEcsSystem<TSystem>;

}
