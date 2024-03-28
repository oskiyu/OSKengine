#pragma once

#include "IRenderSystem.h"

namespace OSK::ECS {

	class OSKAPI_CALL TerrainRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::TerrainRenderSystem");

		TerrainRenderSystem();

		void Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) override;

	};

}
