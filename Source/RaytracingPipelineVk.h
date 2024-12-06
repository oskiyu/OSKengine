#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IRaytracingPipeline.h"
#include "IPipelineVk.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class MaterialLayout;

	class OSKAPI_CALL RaytracingPipelineVk final : public IRaytracingPipeline, public IPipelineVk {

	public:

		RaytracingPipelineVk() = default;

		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		void Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info);

	private:

	};

}

#endif
