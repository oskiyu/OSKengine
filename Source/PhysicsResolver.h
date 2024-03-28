#pragma once

#include "IConsumerSystem.h"
#include "CollisionEvent.h"

namespace OSK::ECS {

	/// @brief Sistema que resuelve las colisiones entre entidades.
	/// @todo Implementación.
	class OSKAPI_CALL PhysicsResolver final : public ITypedConsumerSystem<CollisionEvent> {

	public:

		OSK_SYSTEM("OSK::PhysicsResolverSystem");

		void Execute(TDeltaTime deltaTime, std::span<const CollisionEvent> events) override;

	};

}
