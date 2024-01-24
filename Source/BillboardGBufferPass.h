#pragma once

#include "IRenderPass.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL BillboardGBufferPass : public IRenderPass {

	public:

		BillboardGBufferPass() : IRenderPass("billboard_pass") {}

		void Load() override;

		void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			UIndex32 jitterIndex,
			Vector2ui resolution) override;

	};

}
