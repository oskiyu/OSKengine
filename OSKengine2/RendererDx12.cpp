#include "RendererDx12.h"

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
#include "SyncDeviceDx12.h"
#include "GpuMemoryAllocatorDx12.h"
#include "GpuImageLayout.h"
#include "RenderpassType.h"
#include "Color.hpp"
#include "GraphicsPipelineDx12.h"
#include "PipelineCreateInfo.h"
#include "Vertex.h"
#include "GpuVertexBufferDx12.h"
#include "GpuMemorySubblockDx12.h"
#include "Viewport.h"
#include "GpuIndexBufferDx12.h"
#include "MaterialLayout.h"
#include "GpuUniformBufferDx12.h"
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

#include <glm/ext/matrix_transform.hpp>

using namespace OSK;
using namespace OSK::GRAPHICS;

RendererDx12::RendererDx12(bool requestRayTracing) : IRenderer(RenderApiType::DX12, requestRayTracing) {

}

RendererDx12::~RendererDx12() {
	Close();
}

void RendererDx12::Initialize(const std::string& appName, const Version& version, const IO::IDisplay& display, PresentMode mode) {
	Engine::GetLogger()->InfoLog("Iniciando renderizador DX12.");

	this->display = &display;

#ifdef OSK_RELEASE
	useDebugConsole = false;
#else
	useDebugConsole = SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugConsole)));
#endif

	if (useDebugConsole) {
		debugConsole->EnableDebugLayer();
		debugConsole->SetEnableGPUBasedValidation(true);

		auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
		OSK_CHECK(SUCCEEDED(result), "No se ha podido crear las capas de validación.");
		
		Engine::GetLogger()->InfoLog("Capas de validación activas.");
	}
	else {
		Engine::GetLogger()->InfoLog("Capas de validación no disponibles.");

		CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	}

	ChooseGpu();
	CreateCommandQueues();
	CreateSwapchain(mode);
	CreateSyncDevice();
	CreateGpuMemoryAllocator();

	renderTargetsCamera = new ECS::CameraComponent2D;
	renderTargetsCamera->LinkToDisplay(&display);
	renderTargetsCameraTransform.SetScale({ display.GetResolution().X / 2.0f, display.GetResolution().Y / 2.0f });
	renderTargetsCamera->UpdateUniformBuffer(renderTargetsCameraTransform);

	CreateMainRenderpass();

	if (Engine::GetEcs())
		for (auto i : Engine::GetEcs()->GetRenderSystems())
			i->CreateTargetImage(display.GetResolution());

	isOpen = true;

	HlslRuntimeCompiler::InitializeComponents();
}

void RendererDx12::WaitForCompletion() {

}

void RendererDx12::Close() {
	isOpen = false;
}

void RendererDx12::HandleResize() {
	mustResize = true;

	IRenderer::HandleResize();
}

void RendererDx12::Resize() {
	if (display->GetResolution().X == 0 && display->GetResolution().Y == 0)
		return;

	Format format = swapchain->GetImage(0)->GetFormat();

	swapchain->As<SwapchainDx12>()->DeleteImages();

	swapchain->As<SwapchainDx12>()->GetSwapchain()->ResizeBuffers(swapchain->GetImageCount(),
		display->GetResolution().X, display->GetResolution().Y, GetFormatDx12(format), 0);

	swapchain->As<SwapchainDx12>()->CreateImages(*display);
	//finalRenderpass->SetImages(swapchain->GetImage(0), swapchain->GetImage(1), swapchain->GetImage(2));
}

const TByte* RendererDx12::FormatImageDataForGpu(const GpuImage* image, const TByte* data, TSize numLayers) {
	TByte* output = new TByte[image->GetPhysicalNumberOfBytes() * numLayers];

	const TSize numBytesPerPixel = GetFormatNumberOfBytes(image->GetFormat());
	const TSize numBytesPerLayer = image->GetPhysicalNumberOfBytes();

	for (TSize i = 0; i < numLayers + 0; i++) {
		const TSize layerOffset = numBytesPerLayer * i;

		for (TSize y = 0; y < image->GetPhysicalSize().Y; y++)
			memcpy(&output[layerOffset + y * image->GetPhysicalSize().X * numBytesPerPixel], 
				&data[layerOffset + y * image->GetSize2D().X * numBytesPerPixel], 
				image->GetSize2D().X * numBytesPerPixel);
	}

	return output;
}

OwnedPtr<IGraphicsPipeline> RendererDx12::_CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexInfo) {
	GraphicsPipelineDx12* pipeline = new GraphicsPipelineDx12();

	pipeline->Create(&layout, currentGpu.GetPointer(), pipelineInfo, vertexInfo);

	return pipeline;
}

OwnedPtr<IRaytracingPipeline> RendererDx12::_CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexTypeName) {
	OSK_ASSERT(false, "No implementado.");
	return nullptr;
}

OwnedPtr<IComputePipeline> RendererDx12::_CreateComputePipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout) {
	return new ComputePipelineDx12(pipelineInfo, &layout, currentGpu.GetPointer());
}

OwnedPtr<IMaterialSlot> RendererDx12::_CreateMaterialSlot(const std::string& name, const MaterialLayout& layout) const {
	return new MaterialSlotDx12(name, &layout);
}

void RendererDx12::ChooseGpu() {
	currentGpu = new GpuDx12;
	ComPtr<IDXGIAdapter1> adapter;

	bool found = false;
	// Iteramos sobre todas las gpu disponibles.
	for (uint32_t i = 0; !found && factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		DXGI_ADAPTER_DESC1 description;
		adapter->GetDesc1(&description);

		// que este adaptador no sea emulado por software
		if (description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		// decidir con bAdapterFound si es este adaptador el que queramos 
		auto isValid = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(isValid)) {
			Engine::GetLogger()->InfoLog("GPU elegida: " + std::string(CW2A(description.Description)));

			found = true;
		}
	}

	OSK_ASSERT(found, "No se ha encontrado ninguna GPU compatible con DirectX 12");

	ComPtr<ID3D12Device5> device;
	D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device));
		
	currentGpu->As<GpuDx12>()->SetAdapter(adapter);
	currentGpu->As<GpuDx12>()->SetDevice(device);

	if (useDebugConsole) {
		ID3D12InfoQueue* infoQueue1 = nullptr;
		HRESULT result = device->QueryInterface(IID_ID3D12InfoQueue, (LPVOID*)&infoQueue1);

		if (SUCCEEDED(result)) {
			// result = device->QueryInterface(IID_ID3D12InfoQueue1, (LPVOID*)&debugMessageQueue);
			// debugMessageQueue->RegisterMessageCallback(RendererDx12::DebugCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, NULL);
		}
		else {
			Engine::GetLogger()->InfoLog("NO se ha podido establecer el callback de mensajes de log.");
		}
	}
}

void RendererDx12::CreateCommandQueues() {
	//graphicsQueue = new CommandQueueDx12;

	ComPtr<ID3D12CommandQueue> commandQ;

	D3D12_COMMAND_QUEUE_DESC createInfo{};

	// Hay distintos tipos de colas como:
	//	D3D12_COMMAND_LIST_TYPE_COMPUTE
	// para temas relacionados con compute shader
	// pero nosotros queremos usar la cola gráfica:
	createInfo.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	// para temas de usar múltiples nodos GPU.
	// para uso de una sola GPU se pone a 0
	createInfo.NodeMask = 0;

	// La prioridad de la cola, normalmente NORMAL
	createInfo.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	// Flags adicionales. En estos momentos solo hay dos:
	//	D3D12_COMMAND_LIST_TYPE_BUNDLE
	//	D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT
	createInfo.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// Una vez rellenada la estructura, crear el CommandQueue
	currentGpu->As<GpuDx12>()->GetDevice()->CreateCommandQueue(&createInfo, IID_PPV_ARGS(&commandQ));

	graphicsQueue->As<CommandQueueDx12>()->SetCommandQueue(commandQ);
	graphicsCommandPool = currentGpu->CreateGraphicsCommandPool().GetPointer();

	graphicsCommandList = graphicsCommandPool->CreateCommandList(currentGpu.GetValue()).GetPointer();
	graphicsCommandList->As<CommandListDx12>()->SetCommandPool(*graphicsCommandPool->As<CommandPoolDx12>());
	Engine::GetLogger()->InfoLog("Creada la lista de comandos.");
}

void RendererDx12::CreateSwapchain(PresentMode mode) {
	swapchain = new SwapchainDx12;

	swapchain->As<SwapchainDx12>()->Create(mode, currentGpu.GetPointer(), Format::RGBA8_UNORM, *graphicsQueue->As<CommandQueueDx12>(), factory.Get(), *display);

	Engine::GetLogger()->InfoLog("Creado el swapchain.");
}

void RendererDx12::CreateSyncDevice() {
	syncDevice = currentGpu->As<GpuDx12>()->CreateSyncDevice().GetPointer();
}

void RendererDx12::CreateGpuMemoryAllocator() {
	gpuMemoryAllocator = new GpuMemoryAllocatorDx12(currentGpu.GetPointer());

	Engine::GetLogger()->InfoLog("Creado el asignador de memoria de la GPU.");
}

void RendererDx12::PresentFrame() {
	if (isFirstRender) {
		graphicsCommandList->Reset();
		graphicsCommandList->Start();

		preComputeCommandList->Reset();
		preComputeCommandList->Start();

		postComputeCommandList->Reset();
		postComputeCommandList->Start();

		isFirstRender = false;
	}

	graphicsCommandList->Close();
	preComputeCommandList->Close();
	postComputeCommandList->Close();

	for (TSize i = 0; i < singleTimeCommandLists.GetSize(); i++)
		singleTimeCommandLists.At(i)->DeleteAllStagingBuffers();

	ID3D12CommandList* commandLists[] = { graphicsCommandList->As<CommandListDx12>()->GetCommandList() };
	graphicsQueue->As<CommandQueueDx12>()->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	swapchain->Present();

	syncDevice->As<SyncDeviceDx12>()->Flush(*graphicsQueue->As<CommandQueueDx12>());
	syncDevice->As<SyncDeviceDx12>()->Await();

	graphicsCommandList->Reset();
	preComputeCommandList->Reset();
	postComputeCommandList->Reset();

	//
	if (mustResize) {
		Resize();
		mustResize = false;

		syncDevice->As<SyncDeviceDx12>()->Flush(*graphicsQueue->As<CommandQueueDx12>());
		syncDevice->As<SyncDeviceDx12>()->Await();
		swapchain->As<SwapchainDx12>()->UpdateFrameIndex();
	}

	graphicsCommandList->Start();
	preComputeCommandList->Start();
	postComputeCommandList->Start();
}

void RendererDx12::SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) {
	ID3D12CommandList* commandLists[] = { commandList->As<CommandListDx12>()->GetCommandList() };
	graphicsQueue->As<CommandQueueDx12>()->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	syncDevice->As<SyncDeviceDx12>()->Flush(*graphicsQueue->As<CommandQueueDx12>());
	syncDevice->As<SyncDeviceDx12>()->Await();

	singleTimeCommandLists.Insert(commandList.GetPointer());
}

TSize RendererDx12::GetCurrentFrameIndex() const {
	return 0;
}

TSize RendererDx12::GetCurrentCommandListIndex() const {
	return 0;
}

bool RendererDx12::SupportsRaytracing() const {
	return false;
}

void RendererDx12::DebugCallback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, LPCSTR description, void* context) {
	Engine::GetLogger()->DebugLog(std::string(description));
}
