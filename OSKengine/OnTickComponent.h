#pragma once

#include "Component.h"

#include <functional>

namespace OSK {

	/// <summary>
	/// Componente que permite a una entidad ejecutar la función OnTick().
	/// </summary>
	class OSKAPI_CALL OnTickComponent : public Component {

	public:

		/// <summary>
		/// Función OnTick().
		/// </summary>
		std::function<void(deltaTime_t deltaTime)> OnTick;

	};

}