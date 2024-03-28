#pragma once

#include "IShaderPass.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL AnimatedGBufferPass : public IShaderPass {

	public:
		
		AnimatedGBufferPass() : IShaderPass("animated_pass") {}

		void Load() override;

		void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			GlobalMeshMapping* meshMapping,
			UIndex32 jitterIndex,
			Vector2ui resolution) override;

	};

}
