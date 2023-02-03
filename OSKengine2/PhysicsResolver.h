#pragma once

#include "OSKengine.h"
#include "EntityComponentSystem.h"

#include "IConsumerSystem.h"
#include "CollisionEvent.h"

namespace OSK::ECS {

	/// @brief Sistema que resuelve las colisiones entre entidades.
	/// @todo Implementación.
	class OSKAPI_CALL PhysicsResolver final : public ConsumerSystem<CollisionEvent> {

	public:

		void OnTick(TDeltaTime deltaTime) override {
			EntityComponentSystem* ecs = Engine::GetEcs();

			for (const auto& event : Engine::GetEcs()->GetEventQueue<CollisionEvent>()) {
				const GameObjectIndex first = event.firstEntity;
				const GameObjectIndex second = event.secondEntity;

				if (!(ecs->IsGameObjectAlive(first) && ecs->IsGameObjectAlive(second)))
					continue;
			}
		}

	};

}
