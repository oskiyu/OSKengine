#pragma once

#include "ApiCall.h"
#include "Vector2.hpp"
#include "UniquePtr.hpp"

#include "IPostProcessPass.h"
#include "MaterialInstance.h"


namespace OSK::GRAPHICS {

	class ICommandList;
	class GpuImage;
	class Material;


	/// @brief Efecto de post-procesado que implementa
	/// Fast Approximate Anti-Aliasing.
	class OSKAPI_CALL FxaaPass : public IPostProcessPass {

	public:

		constexpr static auto MaterialName = "Resources/Materials/PostProcess/fxaa.json";

		void Create(const Vector2ui& size) override;
		void Execute(ICommandList* computeCmdList) override;

		void SetInput(GpuImage* image);

	private:

		GpuImage* m_inputImage = nullptr;

		Material* m_material = nullptr;
		UniquePtr<MaterialInstance> m_materialInstance;

	};

}
