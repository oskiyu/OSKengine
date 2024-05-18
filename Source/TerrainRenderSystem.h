#pragma once

#include "IRenderSystem.h"

namespace OSK::ECS {

	class OSKAPI_CALL TerrainRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::TerrainRenderSystem");

		TerrainRenderSystem();

		void Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) override;

		nlohmann::json SaveConfiguration() const override;
		PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const override;
		void ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) override;
		void ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) override;

	};

}
