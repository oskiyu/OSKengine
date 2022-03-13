#pragma once

#include <string>
#include "OSKmacros.h"
#include "IRenderer.h"
#include "UniquePtr.hpp"

#include <wrl.h>
using namespace Microsoft::WRL;
#include <d3d12.h>
#include <dxgi1_6.h>

namespace OSK::GRAPHICS {

	class CommandQueueDx12;

	/// <summary>
	/// Implementación de la interfaz para el renderizador de DirectX 12.
	/// </summary>
	class OSKAPI_CALL RendererDx12 : public IRenderer {

	public:

		~RendererDx12();
		RendererDx12();

		void Initialize(const std::string& appName, const Version& version, const IO::Window& window) override;
		void Close() override;
		void HandleResize() override;

		void PresentFrame() override;
		void SubmitSingleUseCommandList(ICommandList* commandList) override;

		OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const IRenderpass* renderpass) override;
		OwnedPtr<IMaterialSlot> _CreateMaterialSlot(const std::string& name, const MaterialLayout* layout) const override;

	protected:

		void CreateCommandQueues() override;
		void CreateSwapchain() override;
		void CreateSyncDevice() override;
		void CreateGpuMemoryAllocator() override;
		void CreateMainRenderpass() override;

	private:

		void ChooseGpu();

		void Resize();
		bool mustResize = false;

		/// <summary>
		/// Se usa para crear cosas de dx12.
		/// </summary>
		ComPtr<IDXGIFactory4> factory;
		ComPtr<ID3D12Debug> debugConsole;

	};

}
