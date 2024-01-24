#pragma once

#include "IRenderPass.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL TreeNormalsPass : public IRenderPass {

	public:
		
		OSK_RENDERPASS("tree_normals_pass");

		TreeNormalsPass() : IRenderPass("tree_normals_pass") {}

		void Load() override;

		void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			UIndex32 jitterIndex,
			Vector2ui resolution) override;

	private:

		struct PushConstants {
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			Vector2f resolution = Vector2f::Zero;
			float jitterIndex = 0.0f;
		};

	};

}
