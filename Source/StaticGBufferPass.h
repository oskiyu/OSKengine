#pragma once

#include "IShaderPass.h"

namespace OSK::ASSETS {
	class Model3D;
}

namespace OSK::GRAPHICS {
	
	class Mesh3D;


	class OSKAPI_CALL StaticGBufferPass : public IShaderPass {

	public:

		constexpr static auto Name = "static_pass";

		StaticGBufferPass() : IShaderPass("static_pass") {}

		void Load() override;

		void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			GlobalMeshMapping* meshMapping,
			UIndex32 jitterIndex,
			Vector2ui resolution) override;

	};

}
