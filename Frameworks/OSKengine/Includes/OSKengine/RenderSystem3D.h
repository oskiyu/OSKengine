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

		void Render(GRAPHICS::ICommandList* commandList) override;
		
		void SetDirectionalLight(const GRAPHICS::DirectionalLight& dirLight);
		const GRAPHICS::DirectionalLight& GetDirectionalLight() const;

		GRAPHICS::ShadowMap* GetShadowMap();
		GRAPHICS::IGpuUniformBuffer* GetDirLightUniformBuffer() const;

	private:

		void GenerateShadows(GRAPHICS::ICommandList* commandList);
		void RenderScene(GRAPHICS::ICommandList* commandList);

		GRAPHICS::DirectionalLight directionalLight{};
		UniquePtr<GRAPHICS::IGpuUniformBuffer> dirLightUniformBuffer;

		GRAPHICS::ShadowMap shadowMap;

	};

}
