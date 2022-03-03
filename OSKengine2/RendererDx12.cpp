#include "RendererDx12.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <AtlBase.h>
#include <atlconv.h>

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
#include "RenderpassDx12.h"
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
#include "AssetManager.h"
#include "Texture.h"
#include "GpuImageDx12.h"
#include "FormatDx12.h"

#include <ext/matrix_transform.hpp>

using namespace OSK;

GraphicsPipelineDx12* pipeline = nullptr;
GpuVertexBufferDx12* vertexBuffer = nullptr;
GpuIndexBufferDx12* indexBuffer = nullptr;
GpuUniformBufferDx12* uniformBuffer = nullptr;
OwnedPtr<MaterialInstance> materialInstance = nullptr;

glm::mat4 model(1.0f);
float angle = 0.0f;

RendererDx12::RendererDx12() : IRenderer(RenderApiType::DX12) {

}

RendererDx12::~RendererDx12() {
	Close();
}

void RendererDx12::Initialize(const std::string& appName, const Version& version, const Window& window) {
	Engine::GetLogger()->InfoLog("Iniciando renderizador DX12.");

	this->window = &window;

#ifdef OSK_RELEASE
	bool useDebugConsole = false;
#else
	bool useDebugConsole = SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugConsole)));
#endif

	if (useDebugConsole) {
		debugConsole->EnableDebugLayer();
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
	CreateSwapchain();
	CreateSyncDevice();
	CreateGpuMemoryAllocator();
	CreateMainRenderpass();

	Material* mat = GetMaterialSystem()->LoadMaterial("Resources/material.json");

	mat->RegisterRenderpass(renderpass.GetPointer());
	pipeline = mat->GetGraphicsPipeline(renderpass.GetPointer())->As<GraphicsPipelineDx12>();
	materialInstance = mat->CreateInstance();

	DynamicArray<Vertex3D> vertices = {
		{ {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 0.5f, 1.0f } },
		{ {0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 0.0f } },
		{ {0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 0.0f, 0.0f } }
	};

	DynamicArray<TIndexSize> indices = {
		0, 1, 2
	};

	vertexBuffer = gpuMemoryAllocator->CreateVertexBuffer(vertices).GetPointer()->As<GpuVertexBufferDx12>();
	indexBuffer = gpuMemoryAllocator->CreateIndexBuffer(indices).GetPointer()->As<GpuIndexBufferDx12>();
	uniformBuffer = gpuMemoryAllocator->CreateUniformBuffer(sizeof(model))->As<GpuUniformBufferDx12>();
	uniformBuffer->MapMemory();

	Texture* texture = Engine::GetAssetManager()->Load<Texture>("Resources/Assets/texture0.json", "GLOBAL");

	materialInstance->GetSlot("global")->SetUniformBuffer("camera", uniformBuffer);
	materialInstance->GetSlot("global")->SetTexture("texture", texture);
	materialInstance->GetSlot("global")->FlushUpdate();
}

void RendererDx12::Close() {
	
}

void RendererDx12::HandleResize() {
	Format format = swapchain->GetImage(0)->GetFormat();

	swapchain->As<SwapchainDx12>()->DeleteImages();

	swapchain->As<SwapchainDx12>()->GetSwapchain()->ResizeBuffers(swapchain->GetImageCount(),
		window->GetWindowSize().X, window->GetWindowSize().Y, GetFormatDx12(format), 0);

	swapchain->As<SwapchainDx12>()->CreateImages(*window);
}

OwnedPtr<IGraphicsPipeline> RendererDx12::_CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const IRenderpass* renderpass) {
	GraphicsPipelineDx12* pipeline = new GraphicsPipelineDx12();

	pipeline->Create(layout, currentGpu.GetPointer(), pipelineInfo);

	return pipeline;
}

OwnedPtr<IMaterialSlot> RendererDx12::_CreateMaterialSlot(const std::string& name, const MaterialLayout* layout) const {
	return new MaterialSlotDx12(name, layout);
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
		auto isValid = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(isValid)) {
			Engine::GetLogger()->InfoLog("GPU elegida: " + std::string(CW2A(description.Description)));

			found = true;
		}
	}

	OSK_ASSERT(found, "No se ha encontrado ninguna GPU compatible con DirectX 12");

	ComPtr<ID3D12Device> device;
	D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
		
	currentGpu->As<GpuDx12>()->SetAdapter(adapter);
	currentGpu->As<GpuDx12>()->SetDevice(device);
}

void RendererDx12::CreateCommandQueues() {
	graphicsQueue = new CommandQueueDx12;

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

	Engine::GetLogger()->InfoLog("Creada la cola de comandos.");

	commandPool = currentGpu->CreateCommandPool().GetPointer();
	Engine::GetLogger()->InfoLog("Creada el pool de comandos.");

	commandList = commandPool->CreateCommandList(currentGpu.GetValue()).GetPointer();
	commandList->As<CommandListDx12>()->SetCommandPool(*commandPool->As<CommandPoolDx12>());
	Engine::GetLogger()->InfoLog("Creada la lista de comandos.");
}

void RendererDx12::CreateSwapchain() {
	swapchain = new SwapchainDx12;

	swapchain->As<SwapchainDx12>()->Create(currentGpu.GetPointer(), Format::RGBA8_UNORM, *graphicsQueue->As<CommandQueueDx12>(), factory.Get(), *window);

	Engine::GetLogger()->InfoLog("Creado el swapchain.");
}

void RendererDx12::CreateSyncDevice() {
	syncDevice = currentGpu->As<GpuDx12>()->CreateSyncDevice().GetPointer();
}

void RendererDx12::CreateGpuMemoryAllocator() {
	gpuMemoryAllocator = new GpuMemoryAllocatorDx12(currentGpu.GetPointer());

	Engine::GetLogger()->InfoLog("Creado el asignador de memoria de la GPU.");
}

void RendererDx12::CreateMainRenderpass() {
	renderpass = new RenderpassDx12(RenderpassType::FINAL);
	renderpass->SetImages(swapchain->GetImage(0), swapchain->GetImage(1), swapchain->GetImage(2));
	renderpass->As<RenderpassDx12>()->SetSwapchain(swapchain->As<SwapchainDx12>());
}

void RendererDx12::PresentFrame() {
	if (isFirstRender) {
		syncDevice->As<SyncDeviceDx12>()->Flush(*graphicsQueue->As<CommandQueueDx12>());
		syncDevice->As<SyncDeviceDx12>()->Await();

		/*commandList->Reset();
		commandList->Start();
		commandList->BeginAndClearRenderpass(renderpass.GetPointer(), Color::RED());*/

		isFirstRender = false;

		return;
	}

	for (TSize i = 0; i < singleTimeCommandLists.GetSize(); i++)
		singleTimeCommandLists.At(i)->DeleteAllStagingBuffers();

	//
	model = glm::rotate(model, 0.1f, { 1, 0, 0 });
	angle += 0.001f;

	//uniformBuffer->MapMemory();
	//uniformBuffer->ResetCursor();
	//uniformBuffer->Write(&model, sizeof(model));
	//uniformBuffer->Unmap();
	//

	commandList->Reset();
	commandList->Start();

	commandList->BeginAndClearRenderpass(renderpass.GetPointer(), Color::RED());

	commandList->BindMaterial(materialInstance->GetMaterial());

	Vector4ui windowRec = {
		0,
		0,
		window->GetWindowSize().X,
		window->GetWindowSize().Y
	};

	Viewport viewport{};
	viewport.rectangle = windowRec;

	commandList->SetViewport(viewport);
	commandList->SetScissor(windowRec);

	commandList->BindVertexBuffer(vertexBuffer);
	commandList->BindIndexBuffer(indexBuffer);

	commandList->BindMaterialSlot(materialInstance->GetSlot("global"));
	commandList->PushMaterialConstants("model", model);

	commandList->DrawSingleInstance(3);

	commandList->EndRenderpass(renderpass.GetPointer());

	commandList->Close();

	ID3D12CommandList* commandLists[] = { commandList->As<CommandListDx12>()->GetCommandList() };
	graphicsQueue->As<CommandQueueDx12>()->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	swapchain->Present();

	syncDevice->As<SyncDeviceDx12>()->Flush(*graphicsQueue->As<CommandQueueDx12>());
	syncDevice->As<SyncDeviceDx12>()->Await();

}

void RendererDx12::SubmitSingleUseCommandList(ICommandList* commandList) {
	ID3D12CommandList* commandLists[] = { commandList->As<CommandListDx12>()->GetCommandList() };
	graphicsQueue->As<CommandQueueDx12>()->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	singleTimeCommandLists.Insert(commandList);
}
