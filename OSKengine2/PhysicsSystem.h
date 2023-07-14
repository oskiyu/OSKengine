#pragma once

#include "IPureSystem.h"

#include "Vector3.hpp"

namespace OSK::ECS {

	class OSKAPI_CALL PhysicsSystem final : public IPureSystem{

	public:

		OSK_SYSTEM("OSK::PhysicsSystem");

		PhysicsSystem() = default;

		void OnCreate() override;
		void OnTick(TDeltaTime deltaTime) override;

	private:

		Vector3f gravityAccel = { 0.0f, -9.8f, 0.0f };
		Vector3f gravity = { 0.0f, -9.8f, 0.0f };

	};

}
