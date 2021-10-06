#pragma once

#include "DX12.h"
#include "Assert.h"

#include <vector>

constexpr uint32_t NUMBER_OF_FRAMES = 3;

class Device {

public:

	void Create() {
		//Crea el factory.
		auto result = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
		OSK_ASSERT2(IsDxSuccess(result), "Error al crear la factory.", result);

		//Obtener la gpu.
		ComPtr<IDXGIAdapter1> gpu;
		bool found = false;
		for (uint32_t adapterIndex = 0; factory->EnumAdapters1(adapterIndex, &gpu) != DXGI_ERROR_NOT_FOUND; adapterIndex++) {
			DXGI_ADAPTER_DESC1 gpuDesc;
			gpu->GetDesc1(&gpuDesc);

			// No seleccione el adaptador Controlador de representación básica.
			if (gpuDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			// Compruebe si el adaptador admite Direct3D 12, pero no cree
			// aún el dispositivo en sí.
			if (SUCCEEDED(D3D12CreateDevice(gpu.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr))) {
				found = true;

				break;
			}
		}
		OSK_ASSERT(found, "No se ha encontrado GPU compatible.");

		result = D3D12CreateDevice(gpu.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
		OSK_ASSERT2(IsDxSuccess(result), "Error al crear el logical device.", result);

		D3D12_COMMAND_QUEUE_DESC queueCreateInfo{};
		queueCreateInfo.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueCreateInfo.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		result = device->CreateCommandQueue(&queueCreateInfo, IID_PPV_ARGS(&commandQueue));
		OSK_ASSERT2(IsDxSuccess(result), "No se pudo crear la cola de comandos.", result);
		SET_DX12_OBJECT_NAME(commandQueue);

		commandPools.resize(NUMBER_OF_FRAMES);
		for (uint32_t i = 0; i < NUMBER_OF_FRAMES; i++) {
			result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandPools[i]));

			OSK_ASSERT2(IsDxSuccess(result), "No se pudo crear la command queue.", result);
		}
	}

	ID3D12Device* GetDevice() const {
		return device.Get();
	}

	IDXGIFactory4* GetFactory() const {
		return factory.Get();
	}

	ID3D12CommandQueue* GetCommandQueue() const {
		return commandQueue.Get();
	}

	ID3D12GraphicsCommandList* GetGraphicsCommandQueue() const {
		return graphicsCommandQueue.Get();
	}

	ID3D12CommandAllocator* GetCommandPool(uint32_t index) const {
		return commandPools[index].Get();
	}

private:

	ComPtr<ID3D12Device> device;
	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12GraphicsCommandList> graphicsCommandQueue;
	std::vector<ComPtr<ID3D12CommandAllocator>> commandPools;

};
