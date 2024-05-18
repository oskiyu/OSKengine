#pragma once

#include "IIteratorSystem.h"
#include <optional>


namespace OSK::ECS {

	class Transform3D;

	class OSKAPI_CALL TransformApplierSystem final : public IIteratorSystem {

	public:

		OSK_SYSTEM("OSK::TransformApplierSystem");

		TransformApplierSystem() = default;

		void OnCreate() override;
		void Execute(TDeltaTime deltaTime, std::span<const GameObjectIndex> objects) override;

		nlohmann::json SaveConfiguration() const override;
		PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const override;
		void ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) override;
		void ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) override;

	private:

		static void Apply(Transform3D& transform, std::optional<const Transform3D*> parent);

	};

}
