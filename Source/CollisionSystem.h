#pragma once

#include "IIteratorSystem.h"

#include "RayCastResult.h"
#include "Ray.h"

namespace OSK::ECS {

	/// @brief Sistema que se encarga de detectar colisiones entre entidades
	/// y publica los eventos correspondientes.
	/// 
	/// Tipo de evento que publica: CollisionEvent.
	class OSKAPI_CALL CollisionSystem final : public IIteratorSystem {

	public:

		OSK_SYSTEM("OSK::CollisionSystem");

		void OnCreate() override;

		nlohmann::json SaveConfiguration() const override;
		PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const override;
		void ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) override;
		void ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) override;

		void OnExecutionStart() override;
		void Execute(TDeltaTime deltaTime, std::span<const GameObjectIndex> objects) override;

		COLLISION::RayCastResult CastRay(
			const COLLISION::Ray& ray,
			GameObjectIndex sendingObject) const;

	};

}
