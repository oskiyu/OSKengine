#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include <string>
#include "ApiCall.h"
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
		void Initialize(const std::string& appName, const Version& version, const IO::IDisplay& display, PresentMode mode) override;
		void Close() override;
		void HandleResize(const Vector2ui& resolution) override;

		void PresentFrame() override;
		void SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) override;
		void WaitForCompletion() override;

		OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexInfo) override;
		OwnedPtr<IMeshPipeline> _CreateMeshPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout) override;
		OwnedPtr<IRaytracingPipeline> _CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexTypeName) override;
		OwnedPtr<IComputePipeline> _CreateComputePipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout) override;
		OwnedPtr<IMaterialSlot> _CreateMaterialSlot(const std::string& name, const MaterialLayout& layout) const override;

		OwnedPtr<ICommandPool> CreateCommandPool(const ICommandQueue* targetQueueType) override;

		bool SupportsRaytracing() const override;

		USize32 GetCurrentFrameIndex() const override;
		USize32 GetCurrentCommandListIndex() const override;

	protected:

		void CreateCommandQueues() override;
		void CreateSwapchain(PresentMode mode, const Vector2ui& resolution) override;
		void CreateGpuMemoryAllocator() override;

	private:

		static void DebugCallback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, LPCSTR description, void* context);

		/// @throws GpuNotFoundException Si no se encuentra ninguna GPU compatible.
		void CreateDevice();

		void Resize();
		bool mustResize = false;

		bool m_useDebugConsole = false;

		/// <summary>
		/// Se usa para crear cosas de dx12.
		/// </summary>
		ComPtr<IDXGIFactory4> m_factory;
		ComPtr<ID3D12Debug1> m_debugConsole;

		// ID3D12InfoQueue1* debugMessageQueue = nullptr;

	};

}

#endif
