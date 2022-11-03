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

		RendererDx12(bool requestRayTracing);
		~RendererDx12();

		void Initialize(const std::string& appName, const Version& version, const IO::Window& window, PresentMode mode) override;
		void Close() override;
		void HandleResize() override;

		void PresentFrame() override;
		void SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) override;

		OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexInfo) override;
		OwnedPtr<IRaytracingPipeline> _CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexTypeName) override;
		OwnedPtr<IComputePipeline> _CreateComputePipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout) override;
		OwnedPtr<IMaterialSlot> _CreateMaterialSlot(const std::string& name, const MaterialLayout& layout) const override;

		bool SupportsRaytracing() const override;

		TSize GetCurrentFrameIndex() const override;
		TSize GetCurrentCommandListIndex() const override;

	protected:

		const TByte* FormatImageDataForGpu(const GpuImage* image, const TByte* data, TSize numLayers) override;

		void CreateCommandQueues() override;
		void CreateSwapchain(PresentMode mode) override;
		void CreateSyncDevice() override;
		void CreateGpuMemoryAllocator() override;

	private:

		static void DebugCallback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, LPCSTR description, void* context);

		void ChooseGpu();

		void Resize();
		bool mustResize = false;

		bool useDebugConsole = false;

		/// <summary>
		/// Se usa para crear cosas de dx12.
		/// </summary>
		ComPtr<IDXGIFactory4> factory;
		ComPtr<ID3D12Debug> debugConsole;

		ID3D12InfoQueue1* debugMessageQueue = nullptr;

	};

}
