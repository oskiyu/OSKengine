#pragma once

#include "Component.h"

#include <functional>

namespace OSK {

	/// <summary>
	/// Componente que permite a una entidad ejecutar la funci�n OnTick().
	/// </summary>
	class OnTickComponent : public Component {

	public:

		OSK_COMPONENT(OnTickComponent)

		/// <summary>
		/// Funci�n OnTick().
		/// </summary>
		std::function<void(deltaTime_t deltaTime)> OnTick;

	};

}