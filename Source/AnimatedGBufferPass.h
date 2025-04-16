#pragma once

#include "IShaderPass.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL AnimatedGBufferPass : public IShaderPass {

	public:

		constexpr static auto Name = "animated_pass";
		constexpr static auto MaterialPath = "Resources/Materials/PBR/Deferred/deferred_gbuffer_anim.json";

		OSK_RENDERPASS(AnimatedGBufferPass, Name);

		AnimatedGBufferPass() : IShaderPass(Name) {}

		void Load() override;

		void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			GlobalMeshMapping* meshMapping,
			UIndex32 jitterIndex,
			Vector2ui resolution) override;

	};

}
