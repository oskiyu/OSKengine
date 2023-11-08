#pragma once

#include "IRenderPass.h"

namespace OSK::ASSETS {
	class Model3D;
}

namespace OSK::GRAPHICS {
	
	class Mesh3D;


	class OSKAPI_CALL StaticGBufferPass : public IRenderPass {

	public:

		StaticGBufferPass() : IRenderPass("static_pass") {}

		void Load() override;

		void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			UIndex32 jitterIndex,
			Vector2ui resolution) override;

	};

}
