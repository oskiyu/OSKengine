#pragma once

#include "IPostProcessPass.h"

#include "GpuBuffer.h"
#include "ResourcesInFlight.h"

#include <span>
#include <array>

namespace OSK::GRAPHICS {

	class ICommandList;
	class GpuImage;
	class GpuImageView;

	class OSKAPI_CALL HbaoPass : public IPostProcessPass {

	public:

		void Create(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;
		void Execute(ICommandList* computeCmdList) override;

		void SetInput(
			GpuImage* image, 
			const GpuImageViewConfig& viewConfig) override;

		void SetNormalsInput(GpuImage* image);
		void SetDepthInput(GpuImage* image);

		void UpdateCamera(
			const glm::mat4& inverseProjection, 
			const glm::mat4& view,
			float nearPlane);

		void UpdateMaterialInstance() override;

	private:

		Vector2ui CalcualteTargetSize(Vector2ui nativeRes) const;

		void LoadMaterials();
		void SetupBlurChain();

		/// @brief Imagen en la que se guardará el filtro HBAO sin blur.
		RtRenderTarget m_unblurredHbaoTarget{};

		/// @brief Imagen HBAO con el primer pase blur.
		RtRenderTarget m_firstBlurTarget{};

		/// @brief Imagen HBAO con el blur aplicado.
		RtRenderTarget m_secondBlurTarget{};

		Material* m_hbaoMaterial = nullptr;
		UniquePtr<MaterialInstance> m_hbaoMaterialInstance = nullptr;

		Material* m_blurMaterial = nullptr;
		UniquePtr<MaterialInstance> m_blurMaterialInstanceA = nullptr;
		UniquePtr<MaterialInstance> m_blurMaterialInstanceB = nullptr;

		GpuImage* m_normalInput{};
		const IGpuImageView* m_normalInputView{};

		GpuImage* m_depthInput{};
		const IGpuImageView* m_depthInputView{};

		std::array<UniquePtr<GpuBuffer>, MAX_RESOURCES_IN_FLIGHT> m_cameraBuffers{};

		float m_renderSizeRatio = 1.0f;

	};

}
