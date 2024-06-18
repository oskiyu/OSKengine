#pragma once

#include "ApiCall.h"
#include "Vector2.hpp"
#include "UniquePtr.hpp"

#include "IPostProcessPass.h"
#include "MaterialInstance.h"

#include "ResourcesInFlight.h"

#include <array>


namespace OSK::GRAPHICS {

	class ICommandList;
	class IGpuStorageBuffer;
	class GpuBuffer;
	class Material;
	class GpuImage;


	/// @brief Pase de post-procesado que realiza un ajuste
	/// de color.
	class OSKAPI_CALL ToneMappingPass : public IPostProcessPass {

	public:

		/// @brief Ruta del material usado por este pase.
		constexpr static auto MaterialPath = "Resources/Materials/PostProcess/tonemapping.json";

	public:

		void Create(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;
		void Execute(ICommandList* computeCmdList) override;

		void SetInput(GpuImage* input);

		/// @brief (Opcional) establece el buffer que contendrá el
		/// nivel de exposición.
		/// Para poder ajustarlos en la GPU.
		/// @param buffers Buffer con los contenidos de exposición.
		void SetExposureBuffer(const GpuBuffer& buffer);


		/// @brief Establece el nivel de exposición a usar.
		/// @param exposure Nivel de exposición.
		void SetExposure(float exposure);

		/// @return Nivel de exposición actual.
		float GetExposure() const;


		/// @brief Establece el nivel de gamma-correction.
		/// @param gamma Nivel de gamma-correction.
		void SetGamma(float gamma);
		
		/// @return Nivel de gamma-correction actual.
		float GetGamma() const;

	private:

		constexpr static Vector2ui DispatchResolution = { 8, 8 };
		
		constexpr static auto TextureSlotName = "texture";
		constexpr static auto ExposureSlotName = "exposure";
		constexpr static auto ExposureBindingName = "exposure";
		constexpr static auto PushConstantsName = "gamma";

		GpuImage* m_inputImage = nullptr;

		Material* m_material = nullptr;
		std::array<UniquePtr<MaterialInstance>, MAX_RESOURCES_IN_FLIGHT> m_materialInstances{};

		float m_exposure = 6.0f;
		float m_gamma = 2.2f;

	};

}
