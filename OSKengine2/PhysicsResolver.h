#pragma once

#include "IConsumerSystem.h"
#include "CollisionEvent.h"

namespace OSK::ECS {

	/// @brief Sistema que resuelve las colisiones entre entidades.
	/// @todo Implementaci�n.
	class OSKAPI_CALL PhysicsResolver final : public IConsumerSystem {

	public:

		OSK_SYSTEM("OSK::PhysicsResolverSystem");

		void OnTick(TDeltaTime deltaTime) override;

	};

}
