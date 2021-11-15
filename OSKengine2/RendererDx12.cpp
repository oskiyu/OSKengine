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

using namespace OSK;

RendererDx12::~RendererDx12() {
	Close();
}

void RendererDx12::Initialize(const std::string& appName, const Version& version, const Window& window) {
	Engine::GetLogger()->InfoLog("Iniciando renderizador DX12.");

	this->window = &window;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugConsole)))) {
		debugConsole->EnableDebugLayer();
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));

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
}

void RendererDx12::Close() {
	
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
		auto isValid = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(isValid)) {
			Engine::GetLogger()->InfoLog("GPU elegida: " + std::string(CW2A(description.Description)));

			found = true;
		}
	}

	OSK_ASSERT(found, "No se ha encontrado ninguna GPU compatible con DirectX 12");

	ComPtr<ID3D12Device> device;
	D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

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

	swapchain->As<SwapchainDx12>()->Create(Format::RGBA8_UNORM, *graphicsQueue->As<CommandQueueDx12>(), factory.Get(), *window);

	Engine::GetLogger()->InfoLog("Creado el swapchain.");
}

void RendererDx12::CreateSyncDevice() {
	syncDevice = currentGpu->As<GpuDx12>()->CreateSyncDevice().GetPointer();
}

void RendererDx12::PresentFrame() {
	syncDevice->As<SyncDeviceDx12>()->Await();

	commandList->Close();

	ID3D12CommandList* commandLists[] = {commandList->As<CommandListDx12>()->GetCommandList()};
	graphicsQueue->As<CommandQueueDx12>()->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	swapchain->As<SwapchainDx12>()->GetSwapchain()->Present(1, 0);

	syncDevice->As<SyncDeviceDx12>()->Flush(*graphicsQueue->As<CommandQueueDx12>());

	commandList->Reset();
	commandList->Start();
}
