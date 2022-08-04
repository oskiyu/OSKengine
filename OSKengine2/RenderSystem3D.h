#pragma once

#include "IRenderSystem.h"

#include "ShadowMap.h"
#include "Lights.h"
#include "IGpuUniformBuffer.h"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {

	/// <summary>
	/// Sistema que se encarga del renderizado de modelos 3D de los objetos.
	/// 
	/// Signature:
	/// - ModelComponent3D.
	/// - Transform3D.
	/// </summary>
	class OSKAPI_CALL RenderSystem3D : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::RenderSystem3D");

		RenderSystem3D();

		void CreateTargetImage(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void Render(GRAPHICS::ICommandList* commandList) override;
		
		void SetDirectionalLight(const GRAPHICS::DirectionalLight& dirLight);
		const GRAPHICS::DirectionalLight& GetDirectionalLight() const;

		GRAPHICS::ShadowMap* GetShadowMap();
		GRAPHICS::IGpuUniformBuffer* GetDirLightUniformBuffer() const;

		const GRAPHICS::RenderTarget& GetBloomRenderTarget() const;

	private:

		void SetupBloomMaterialSlots();

		void GenerateShadows(GRAPHICS::ICommandList* commandList);
		void RenderScene(GRAPHICS::ICommandList* commandList);
		void BlurBloom(GRAPHICS::ICommandList* commandList);

		GRAPHICS::Material* bloomMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> bloomMaterialInstances[3]{ nullptr };

		GRAPHICS::Material* bloomResolveMaterial = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> bloomResolveMaterialInstance = nullptr;

		GRAPHICS::DirectionalLight directionalLight{};
		UniquePtr<GRAPHICS::IGpuUniformBuffer> dirLightUniformBuffer;

		GRAPHICS::ShadowMap shadowMap;

		GRAPHICS::RenderTarget bloomRenderTarget[2]{};
		GRAPHICS::RenderTarget bloomResolveRenderTarget;

	};

}
