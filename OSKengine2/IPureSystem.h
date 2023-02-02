#pragma once

#include "IIteratorSystem.h"

namespace OSK::ECS {

	/// @brief Un sistema puro es un sistema iterador que ni emite ni recibe eventos.
	///
	/// Se ejecutan después de haber terminado la ejecución todos los sistemas productores y consumidores.
	class OSKAPI_CALL IPureSystem : public IIteratorSystem {

	public:

		virtual ~IPureSystem() = default;

	};


	/// @brief Concepto que permite saber si una clase
	/// corresponde a un sistema puro.
	template<typename TSystem>
	concept IsPureSystem = std::is_base_of_v<IPureSystem, TSystem>
		&& IsEcsSystem<TSystem>;

}
