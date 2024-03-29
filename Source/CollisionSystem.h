#pragma once

#include "IProducerSystem.h"

#include "RayCastResult.h"
#include "Ray.h"

namespace OSK::ECS {

	/// @brief Sistema que se encarga de detectar colisiones entre entidades
	/// y publica los eventos correspondientes.
	/// 
	/// Tipo de evento que publica: CollisionEvent.
	class OSKAPI_CALL CollisionSystem final : public IProducerSystem {

	public:

		OSK_SYSTEM("OSK::CollisionSystem");

		void OnCreate() override;
		void OnTick(TDeltaTime deltaTime) override;

		COLLISION::RayCastResult CastRay(
			const COLLISION::Ray& ray,
			GameObjectIndex sendingObject) const;

	};

}
