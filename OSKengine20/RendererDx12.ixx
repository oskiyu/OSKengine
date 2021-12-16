module;

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include <AtlBase.h>
#include <atlconv.h>
#include <string>

export module OSKengine.Graphics.RendererDx12;

export import OSKengine.Graphics.IRenderer;

import OSKengine.Logger;
import OSKengine.Graphics.GpuDx12;
import OSKengine.Graphics.CommandQueueDx12;

export namespace OSK {

	/// <summary>
	/// Renderizador que usa la API DirectX 12.
	/// </summary>
	class RendererDx12 : public IRenderer {

	public:

		void Initialize(const std::string& appName, const Version& version, const Window& window) override {
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugConsole)))) {
				debugConsole->EnableDebugLayer();
				CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));

				GetLogger()->InfoLog("Capas de validación activas.");
			}
			else {
				CreateDXGIFactory1(IID_PPV_ARGS(&factory));
			}

			ChooseGpu();
			CreateCommandQueue();
		}

		void Close() override {

		}

	private:

		/// <summary>
		/// Elige la gpu que se va a usar.
		/// </summary>
		void ChooseGpu() {
			currentGpu = new GpuDx12();
			ComPtr<IDXGIAdapter1> adapter;

			bool found = false;
			// Iteramos sobre todas las gpu disponibles.
			for (uint32_t i = 0; !found && factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
				DXGI_ADAPTER_DESC1 description;
				adapter->GetDesc1(&description);

				// que este adaptador no sea emulado por software
				if (description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
					continue;
				}

				// decidir con bAdapterFound si es este adaptador el que queramos 
				auto isValid = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
				if (SUCCEEDED(isValid)) {
					GetLogger()->InfoLog("GPU elegida: " + std::string(CW2A(description.Description)));

					found = true;
				}
			}
			
			ComPtr<ID3D12Device> device;
			D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

			currentGpu->As<GpuDx12>()->SetAdapter(adapter);
			currentGpu->As<GpuDx12>()->SetDevice(device);
		}

		void CreateCommandQueue() {
			commandQueue = new CommandQueueDx12();

			ComPtr<ID3D12CommandQueue> commandQ;

			D3D12_COMMAND_QUEUE_DESC createInfo{};

			// Hay distintos tipos de colas como:
			//   D3D12_COMMAND_LIST_TYPE_COMPUTE
			// para temas relacionados con compute shader
			// pero nosotros queremos usar la cola gráfica:
			createInfo.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

			// para temas de usar múltiples nodos GPU.
			// para uso de una sola GPU se pone a 0
			createInfo.NodeMask = 0;

			// La prioridad de la cola, normalmente NORMAL
			createInfo.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

			// Flags adicionales. En estos momentos solo hay dos:
			//   D3D12_COMMAND_LIST_TYPE_BUNDLE
			//   D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT
			createInfo.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

			// Una vez rellenada la estructura, crear el CommandQueue
			currentGpu->As<GpuDx12>()->GetDevice()->CreateCommandQueue(&createInfo, IID_PPV_ARGS(&commandQ));

			commandQueue->SetCommandQueue(commandQ);

			GetLogger()->InfoLog("Creada la cola de comandos.");
		}

		/// <summary>
		/// Se usa para crear cosas de dx12.
		/// </summary>
		ComPtr<IDXGIFactory4> factory;

		ComPtr<ID3D12Debug> debugConsole;

		UniquePtr<CommandQueueDx12> commandQueue;

	};

}
