#pragma once

#include "IConsumerSystem.h"
#include "CollisionEvent.h"

namespace OSK::ECS {

	/// @brief Sistema que resuelve las colisiones entre entidades.
	class OSKAPI_CALL PhysicsResolver final : public ITypedConsumerSystem<CollisionEvent> {

	public:

		OSK_SYSTEM("OSK::PhysicsResolverSystem");

		void Execute(TDeltaTime deltaTime, std::span<const CollisionEvent> events) override;

		nlohmann::json SaveConfiguration() const override;
		PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const override;

		void ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) override;
		void ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) override;

	};

}
