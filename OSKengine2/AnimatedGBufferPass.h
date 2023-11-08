#pragma once

#include "IRenderPass.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL AnimatedGBufferPass : public IRenderPass {

	public:
		
		AnimatedGBufferPass() : IRenderPass("animated_pass") {}

		void Load() override;

		void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			UIndex32 jitterIndex,
			Vector2ui resolution) override;

	};

}
