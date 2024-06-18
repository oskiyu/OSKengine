#pragma once

#include "IRenderSystem.h"
#include "SpriteRenderer.h"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {

	class OSKAPI_CALL RenderSystem2D : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::RenderSystem2D");

		RenderSystem2D();

		void Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) override;

		nlohmann::json SaveConfiguration() const override;
		PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const override;
		void ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) override;
		void ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) override;

	private:

		GRAPHICS::SpriteRenderer spriteRenderer;

	};

}
