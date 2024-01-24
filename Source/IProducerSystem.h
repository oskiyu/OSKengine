#pragma once

#include "IIteratorSystem.h"

namespace OSK::ECS {

	/// @brief Un sistema productor es un sistema iterador que puede emitir eventos.
	///
	/// Se ejecutan en primer ligar, antes que los sistemas consumidores y puros.
	class OSKAPI_CALL IProducerSystem : public IIteratorSystem {

	public:

		virtual ~IProducerSystem() = default;

	};


	/// @brief Concepto que permite saber si una clase
	/// corresponde a un sistema productor.
	template<typename TSystem>
	concept IsProducerSystem = std::is_base_of_v<IProducerSystem, TSystem>
		&& IsEcsSystem<TSystem>;

}
