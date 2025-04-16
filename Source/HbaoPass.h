#pragma once

#include "IPostProcessPass.h"

#include "GpuBuffer.h"
#include "ResourcesInFlight.h"

#include <span>
#include <array>
#include "MaterialInstance.h"
#include "UniquePtr.hpp"

namespace OSK::GRAPHICS {

	class ICommandList;
	class GpuImage;
	class GpuImageView;

	class OSKAPI_CALL HbaoPass : public IPostProcessPass {

	public:

		constexpr static auto HbaoMaterial = "Resources/Materials/PostProcess/HBAO/hbao.json";
		constexpr static auto BlurMaterial = "Resources/Materials/PostProcess/HBAO/blur.json";
		constexpr static auto ResolveMaterial = "Resources/Materials/PostProcess/HBAO/resolve.json";

		void Create(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;
		void Execute(ICommandList* computeCmdList) override;

		void SetColorInput(GpuImage* image);
		void SetNormalsInput(GpuImage* image);
		void SetDepthInput(GpuImage* image);

		void UpdateCamera(
			const glm::mat4& inverseProjection, 
			const glm::mat4& view,
			float nearPlane);

	private:

		Vector2ui CalcualteTargetSize(Vector2ui nativeRes) const;

		void LoadMaterials();
		void SetupBlurChain();

		/// @brief Imagen en la que se guardará el filtro HBAO sin blur.
		RtRenderTarget m_unblurredHbaoTarget{};

		/// @brief Imagen intermedia con el primer pase blur.
		RtRenderTarget m_firstBlurTarget{};

		/// @brief Imagen intermedia con el segundo pase blur.
		RtRenderTarget m_secondBlurTarget{};


		/// @brief Material que calcula la oclusión ambiental.
		Material* m_hbaoMaterial = nullptr;

		/// @brief Instancias de generación de oclusión ambiental.
		/// Hay varias debido a que contiene información de la cámara,
		/// que se actualiza desde la CPU.
		::std::array<UniquePtr<MaterialInstance>, MAX_RESOURCES_IN_FLIGHT> m_hbaoMaterialInstances{};


		/// @brief Material para el difuminado del resultado.
		Material* m_blurMaterial = nullptr;

		/// @brief Instancia para el difuminado 1.
		UniquePtr<MaterialInstance> m_blurMaterialInstanceA;
		/// @brief Instancia para el difuminado 2.
		UniquePtr<MaterialInstance> m_blurMaterialInstanceB;


		/// @brief Material que genera la imagen final
		/// combinando la imagen HBAO con la imagen de la escena.
		Material* m_resolveMaterial = nullptr;

		UniquePtr<MaterialInstance> m_resolveMaterialInstance;


		/// @brief Imagen de entrada de la escena.
		GpuImage* m_colorInput{};

		/// @brief Imagen de entrada de normales.
		GpuImage* m_normalInput{};

		/// @brief Imagen de entrada con la profundidad.
		GpuImage* m_depthInput{};



		/// @brief Buffers con la información de la cámara.
		/// Hay varias debido a que contiene información
		/// que se actualiza desde la CPU.
		std::array<UniquePtr<GpuBuffer>, MAX_RESOURCES_IN_FLIGHT> m_cameraBuffers{};


		/// @brief Ratio de resolución.
		/// @invariant > 0.0f
		/// @invariant <= 1.0f
		float m_renderSizeRatio = 1.0f;

	};

}
