#pragma once

#include "ECS.h"

namespace OSK {

	class OnTickSystem : public ECS::System {

	public:

		void OnTick(deltaTime_t deltaTime) override;
		Signature GetSystemSignature() override;

	};

}