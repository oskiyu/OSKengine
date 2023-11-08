#pragma once

#include "IRenderPass.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL TreeGBufferPass : public IRenderPass {

	public:

		OSK_RENDERPASS("tree_pass");

		TreeGBufferPass() : IRenderPass("tree_pass") {}

		void Load() override;

		void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			UIndex32 jitterIndex,
			Vector2ui resolution) override;

	};

}
