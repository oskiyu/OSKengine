#pragma once

#include "IShaderPass.h"


namespace OSK::GRAPHICS {

	class ShadowMap;

	class OSKAPI_CALL ShadowsStaticPass : public IShadowsPass {

	public:

		constexpr static auto Name = "shadows_static_pass";

		ShadowsStaticPass() : IShadowsPass(Name) {}

		void SetupShadowMap(const ShadowMap& shadowMap);

		void Load() override;
		void SetupMaterialInstance(
			const GpuModel3D& model,
			const GpuMesh3D& mesh) override {}

		void ShadowsRenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			UIndex32 cascadeIndex,
			const ShadowMap& shadowMap) override;

	private:

		bool m_hasBeenSet = false;
		UniquePtr<MaterialInstance> m_materialInstance;

	};

}
