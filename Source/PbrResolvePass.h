#pragma once

#include "IDeferredResolver.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL PbrResolverPass : public IDeferredResolver {

	public:

		PbrResolverPass() : IDeferredResolver("pbr_compute_resolver_pass") {}

		void Load() override final;

		void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			GlobalMeshMapping* meshMapping,
			UIndex32 jitterIndex,
			Vector2ui resolution) override final;

	};

}
