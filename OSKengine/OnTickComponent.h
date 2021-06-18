#pragma once

#include "Component.h"

#include <functional>

namespace OSK {

	/// <summary>
	/// Componente que permite a una entidad ejecutar la función OnTick().
	/// </summary>
	class OnTickComponent : public Component {

	public:

		OSK_COMPONENT(OnTickComponent)

		/// <summary>
		/// Función OnTick().
		/// </summary>
		std::function<void(deltaTime_t deltaTime)> OnTick;

	};

}