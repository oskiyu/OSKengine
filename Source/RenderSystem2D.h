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

	private:

		GRAPHICS::SpriteRenderer spriteRenderer{};

	};

}
