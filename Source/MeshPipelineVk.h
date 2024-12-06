#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IMeshPipeline.h"
#include "IPipelineVk.h"
#include <string>
#include "DynamicArray.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuVk;

	class OSKAPI_CALL MeshPipelineVk final : public IMeshPipeline, public IPipelineVk {

	public:

		explicit MeshPipelineVk();

		void Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info) override;

		void SetDebugName(const std::string& name) override;

	private:

		/// @throws FileNotFoundException si no se encuentra el archivo del shader.
		void LoadAmplificationShader(const std::string& path);
		/// @throws FileNotFoundException si no se encuentra el archivo del shader.
		void LoadMeshShader(const std::string& path);
		/// @throws FileNotFoundException si no se encuentra el archivo del shader.
		void LoadFragmentShader(const std::string& path);

		GpuVk* m_gpu = nullptr;

	};

}

#endif
