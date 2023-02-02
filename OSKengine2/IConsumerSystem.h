#pragma once

#include "ISystem.h"
#include "Event.h"

namespace OSK::ECS {

	/// @brief Interfaz com�n para todos los sistemas consumidores.
	class IConsumerSystem : public ISystem {

	public:

		virtual ~IConsumerSystem() = default;

	protected:

		IConsumerSystem() = default;

	};

	/// @brief Un sistema consumidor es un sistema que se encarga de responder a eventos.
	/// Estos sistemas se ejecutan despu�s de los sistemas productores y antes que los sistemas puros.
	/// @tparam TEvent Tipo de evento al que est� suscrito.
	template <typename TEvent> requires IsEcsEvent<TEvent>
	class OSKAPI_CALL ConsumerSystem : public IConsumerSystem {

	public:

		virtual ~ConsumerSystem() = default;

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
