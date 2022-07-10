#pragma once

#include "OSKmacros.h"
#include "IRenderer.h"

namespace OSK::GRAPHICS {

	/// @deprecated OpenGL no implementado.
	class OSKAPI_CALL RendererOgl : public IRenderer {

	public:

		RendererOgl();

		void Initialize(const std::string& appName, const Version& version, const IO::Window& window, PresentMode mode) override;

		void HandleResize() override;
		void Close() override;
		void PresentFrame() override;
		void SubmitSingleUseCommandList(ICommandList* commandList) override;

		bool SupportsRaytracing() const override;

		virtual OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, Format format, const VertexInfo& vertexInfo) override;
		OwnedPtr<IRaytracingPipeline> _CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const VertexInfo& vertexTypeName) override;
		OwnedPtr<IMaterialSlot> _CreateMaterialSlot(const std::string& name, const MaterialLayout* layout) const override;

	protected:

		void CreateCommandQueues() override;
		void CreateSwapchain(PresentMode mode) override;
		void CreateSyncDevice() override;
		void CreateGpuMemoryAllocator() override;

	private:

	};

}
