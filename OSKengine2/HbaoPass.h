#pragma once

#include "IPostProcessPass.h"

#include "GpuBuffer.h"

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
			std::span<GpuImage*, NUM_RESOURCES_IN_FLIGHT> images, 
			const GpuImageViewConfig& viewConfig) override;

		void SetNormalsInput(std::span<GpuImage*, NUM_RESOURCES_IN_FLIGHT> images);
		void SetDepthInput(std::span<GpuImage*, NUM_RESOURCES_IN_FLIGHT> images);

		void UpdateCamera(
			const glm::mat4& inverseProjection, 
			const glm::mat4& view,
			float nearPlane);

		void UpdateMaterialInstance() override;

	private:

		void LoadMaterials();
		void SetupBlurChain();

		/// @brief Imagen en la que se guardará el filtro HBAO sin blur.
		RtRenderTarget unblurredHbaoTarget{};

		/// @brief Imagen HBAO con el primer pase blur.
		RtRenderTarget firstBlurTarget{};

		/// @brief Imagen HBAO con el blur aplicado.
		RtRenderTarget secondBlurTarget{};

		Material* hbaoMaterial = nullptr;
		UniquePtr<MaterialInstance> hbaoMaterialInstance = nullptr;

		Material* blurMaterial = nullptr;
		UniquePtr<MaterialInstance> blurMaterialInstanceA = nullptr;
		UniquePtr<MaterialInstance> blurMaterialInstanceB = nullptr;

		std::array<GpuImage*, NUM_RESOURCES_IN_FLIGHT> m_normalInputs{};
		std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> m_normalInputViews{};

		std::array<GpuImage*, NUM_RESOURCES_IN_FLIGHT> m_depthInputs{};
		std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> m_depthInputViews{};

		std::array<UniquePtr<GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_cameraBuffers{};

	};

}
