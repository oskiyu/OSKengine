#pragma once

#include "Component.h"

#include <functional>

namespace OSK {

	class OSKAPI_CALL OnTickComponent : public Component {

	public:

		std::function<void(deltaTime_t deltaTime)> OnTick;

	};

}