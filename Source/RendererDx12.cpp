#include "RendererDx12.h"

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "HlslRuntimeCompiler.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <AtlBase.h>
#include <atlconv.h>

#undef max

#include <limits>

#include "OSKengine.h"
#include "Logger.h"
#include "GpuDx12.h"
#include "CommandQueueDx12.h"
#include "Assert.h"
#include "CommandPoolDx12.h"
#include "CommandListDx12.h"
#include "SwapchainDx12.h"
#include "Window.h"
#include "Format.h"
#include "Version.h"
#include "GpuMemoryAllocatorDx12.h"
#include "GpuImageLayout.h"
#include "RenderpassType.h"
#include "Color.hpp"
#include "GraphicsPipelineDx12.h"
#include "PipelineCreateInfo.h"
#include "Vertex.h"
#include "GpuBuffer.h"
#include "GpuMemorySubblockDx12.h"
#include "Viewport.h"
#include "MaterialLayout.h"
#include "ShaderBindingType.h"
#include "RenderApiType.h"
#include "Material.h"
#include "MaterialSystem.h"
#include "MaterialInstance.h"
#include "MaterialSlotDx12.h"
#include "GpuImageDx12.h"
#include "FormatDx12.h"
#include "IGpuImage.h"

#include "ComputePipelineDx12.h"
#include "Texture.h"
#include "Model3D.h"
#include "AssetManager.h"

#include "RendererExceptions.h"

#include <glm/ext/matrix_transform.hpp>
#include <dxgi.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

RendererDx12::RendererDx12(bool requestRayTracing) : IRenderer(RenderApiType::DX12, requestRayTracing) {

}

RendererDx12::~RendererDx12() {
	Close();
}

void RendererDx12::Initialize(const std::string& appName, const Version& version, const IO::IDisplay& display, PresentMode mode) {
	Engine::GetLogger()->InfoLog("Iniciando renderizador DX12.");

#ifdef OSK_RELEASE
	m_useDebugConsole = false;
#else
	m_useDebugConsole = SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugConsole)));
#endif

	if (m_useDebugConsole) {
		m_debugConsole->EnableDebugLayer();
		m_debugConsole->SetEnableGPUBasedValidation(true);

		auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_factory));

		if (!SUCCEEDED(result)) {
			Engine::GetLogger()->InfoLog("No se ha podido crear las capas de validación.");
		}
		else {
			Engine::GetLogger()->InfoLog("Capas de validación activas.");
		}
	}
	else {
		Engine::GetLogger()->InfoLog("Capas de validación no disponibles.");

		CreateDXGIFactory1(IID_PPV_ARGS(&m_factory));
	}

	CreateDevice();
	CreateCommandQueues();
	CreateSwapchain(mode, Engine::GetDisplay()->GetResolution());
	CreateGpuMemoryAllocator();

	CreateMainRenderpass();

	if (Engine::GetEcs())
		for (auto i : Engine::GetEcs()->GetRenderSystems())
			i->CreateTargetImage(display.GetResolution());

	HlslRuntimeCompiler::InitializeComponents();
}

void RendererDx12::WaitForCompletion() {

}

void RendererDx12::Close() {

}

void RendererDx12::HandleResize(const Vector2ui& resolution) {
	mustResize = true;

	IRenderer::HandleResize(resolution);
}

void RendererDx12::Resize() {
	const auto* display = Engine::GetDisplay();

	if (display->GetResolution().x == 0 && display->GetResolution().y == 0)
		return;

	Format format = _GetSwapchain()->GetImage(0)->GetFormat();

	_GetSwapchain()->As<SwapchainDx12>()->DeleteImages();

	_GetSwapchain()->As<SwapchainDx12>()->GetSwapchain()->ResizeBuffers(
		_GetSwapchain()->GetImageCount(),
		display->GetResolution().x, 
		display->GetResolution().y, 
		GetFormatDx12(format), 0);

	_GetSwapchain()->As<SwapchainDx12>()->CreateImages(*display);
	//finalRenderpass->SetImages(swapchain->GetImage(0), swapchain->GetImage(1), swapchain->GetImage(2));
}

OwnedPtr<IGraphicsPipeline> RendererDx12::_CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexInfo) {
	GraphicsPipelineDx12* pipeline = new GraphicsPipelineDx12();

	pipeline->Create(&layout, GetGpu(), pipelineInfo, vertexInfo);

	return pipeline;
}

OwnedPtr<IMeshPipeline> RendererDx12::_CreateMeshPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout) {
	OSK_ASSERT(false, NotImplementedException());
	return nullptr;
}

OwnedPtr<IRaytracingPipeline> RendererDx12::_CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexTypeName) {
	OSK_ASSERT(false, NotImplementedException());
	return nullptr;
}

OwnedPtr<IComputePipeline> RendererDx12::_CreateComputePipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout) {
	return new ComputePipelineDx12(pipelineInfo, &layout, GetGpu());
}

OwnedPtr<IMaterialSlot> RendererDx12::_CreateMaterialSlot(const std::string& name, const MaterialLayout& layout) const {
	return new MaterialSlotDx12(name, &layout);
}

OwnedPtr<ICommandPool> RendererDx12::CreateCommandPool(const ICommandQueue* targetQueueType) {
	return new CommandPoolDx12(
		GetGpu()->As<GpuDx12>(),
		targetQueueType->GetSupportedCommands(),
		targetQueueType->GetQueueType());
}

void RendererDx12::CreateDevice() {
	auto gpu = new GpuDx12;
	_SetGpu(gpu);

	auto adapter = GpuDx12::ChooseDeviceAdapter(m_factory);
	gpu->CreateDevice(adapter, m_useDebugConsole);
}

void RendererDx12::CreateCommandQueues() {
	// Usar cola unificada.
	_SetUnifiedCommandQueue(new CommandQueueDx12(GetGpu()->As<GpuDx12>(), GpuQueueType::MAIN));

	RegisterUnifiedCommandPool(GetUnifiedQueue());

	_SetMainCommandList(GetUnifiedCommandPool(std::this_thread::get_id())->CreateCommandList(*GetGpu()));

	OSK::Engine::GetLogger()->InfoLog("Uso de cola GPU unificada.");
	
	// Transfer queue.
	_SetTransferOnlyCommandQueue(new CommandQueueDx12(GetGpu()->As<GpuDx12>(), GpuQueueType::ASYNC_TRANSFER));
	RegisterTransferOnlyCommandPool(GetTransferOnlyQueue());
	OSK::Engine::GetLogger()->InfoLog("Uso de cola GPU de transferencia.");
}

void RendererDx12::CreateSwapchain(PresentMode mode, const Vector2ui& resolution) {
	_SetSwapchain(
		new SwapchainDx12(
		GetUnifiedQueue()->As<CommandQueueDx12>(),
		mode,
		Format::RGBA8_UNORM,
		GetGpu()->As<GpuDx12>(),
		*Engine::GetDisplay(),
		m_factory.Get()));
}

void RendererDx12::CreateGpuMemoryAllocator() {
	_SetMemoryAllocator(new GpuMemoryAllocatorDx12(GetGpu()));
}

void RendererDx12::PresentFrame() {
	if (m_isFirstRender) {
		GetMainCommandList()->Reset();
		GetMainCommandList()->Start();

		m_isFirstRender = false;
	}

	GetMainCommandList()->Close();

	for (auto& cmdList : m_singleTimeCommandLists)
		cmdList->DeleteAllStagingBuffers();

	ID3D12CommandList* commandList = GetMainCommandList()->As<CommandListDx12>()->GetCommandList();
	GetUnifiedQueue()->As<CommandQueueDx12>()->GetCommandQueue()->ExecuteCommandLists(1, &commandList);

	_GetSwapchain()->Present();

	// syncDevice->As<SyncDeviceDx12>()->Flush(*graphicsQueue->As<CommandQueueDx12>());
	// syncDevice->As<SyncDeviceDx12>()->Await();

	GetMainCommandList()->Reset();

	//
	if (mustResize) {
		Resize();
		mustResize = false;

		// syncDevice->As<SyncDeviceDx12>()->Flush(*graphicsQueue->As<CommandQueueDx12>());
		// syncDevice->As<SyncDeviceDx12>()->Await();
		_GetSwapchain()->As<SwapchainDx12>()->UpdateFrameIndex();
	}

	GetMainCommandList()->Start();
}

void RendererDx12::SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) {
	ID3D12CommandList* commandLists[] = { commandList->As<CommandListDx12>()->GetCommandList() };
	GetGraphicsComputeQueue()->As<CommandQueueDx12>()->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	// syncDevice->As<SyncDeviceDx12>()->Flush(*graphicsQueue->As<CommandQueueDx12>());
	// syncDevice->As<SyncDeviceDx12>()->Await();

	m_singleTimeCommandLists.Insert(commandList.GetPointer());
}

USize32 RendererDx12::GetCurrentFrameIndex() const {
	return 0;
}

UIndex32 RendererDx12::GetCurrentCommandListIndex() const {
	return 0;
}

bool RendererDx12::SupportsRaytracing() const {
	return false;
}

void RendererDx12::DebugCallback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, LPCSTR description, void* context) {
	Engine::GetLogger()->DebugLog(std::string(description));
}

#endif
