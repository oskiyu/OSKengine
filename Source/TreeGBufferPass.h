#pragma once

#include "IShaderPass.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL TreeGBufferPass : public IShaderPass {

	public:

		OSK_RENDERPASS("tree_pass");

		TreeGBufferPass() : IShaderPass("tree_pass") {}

		void Load() override;

		void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			GlobalMeshMapping* meshMapping,
			UIndex32 jitterIndex,
			Vector2ui resolution) override;

	};

}
