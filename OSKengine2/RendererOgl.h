#pragma once

#include "OSKmacros.h"
#include "IRenderer.h"

namespace OSK {

	/// <summary>
	/// Implementación de la interfaz para el renderizador de OpenGL.
	/// </summary>
	class OSKAPI_CALL RendererOgl : public IRenderer {

	public:

		RendererOgl();

		void Initialize(const std::string& appName, const Version& version, const Window& window) override;

		void HandleResize() override;
		void Close() override;
		void PresentFrame() override;
		void SubmitSingleUseCommandList(ICommandList* commandList) override;

		virtual OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const IRenderpass* renderpass) override;
		OwnedPtr<IMaterialSlot> _CreateMaterialSlot(const std::string& name, const MaterialLayout* layout) const override;

	protected:

		void CreateCommandQueues() override;
		void CreateSwapchain() override;
		void CreateSyncDevice() override;
		void CreateGpuMemoryAllocator() override;
		void CreateMainRenderpass() override;

	private:

	};

}
