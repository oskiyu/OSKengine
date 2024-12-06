#pragma once

#include "IIteratorSystem.h"

#include "Vector3.hpp"


namespace OSK::ECS {

	class OSKAPI_CALL PhysicsSystem final : public IIteratorSystem, public ISerializableSystem {

	public:

		OSK_SYSTEM("OSK::PhysicsSystem");

		PhysicsSystem() = default;

		void OnCreate() override;
		void Execute(TDeltaTime deltaTime, std::span<const GameObjectIndex> objects) override;

		nlohmann::json SaveConfiguration() const override;
		PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const override;
		void ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) override;
		void ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) override;

		const Vector3f& GetGravity() const;

	private:

		Vector3f m_gravity = { 0.0f, -2.7f, 0.0f };

	};

}
