#pragma once

#include "Engine.h"
#include "Device.h"
#include "MemoryAllocator.h"

class Swapchain {

public:

private:

	typedef Platform::Agile<Windows::UI::Core::CoreWindow> Window_t;

	void Create(Device* device, Window_t* window, uint32_t sizeX, uint32_t sizeY, DXGI_FORMAT format) {
		DXGI_SCALING scaling = DXGI_SCALING_NONE;

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
		swapchainDesc.Width = sizeX;						
		swapchainDesc.Height = sizeY;
		swapchainDesc.Format = format;
		swapchainDesc.Stereo = false;
		swapchainDesc.SampleDesc.Count = 1;							
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.BufferCount = NUMBER_OF_FRAMES;					
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.Flags = 0;
		swapchainDesc.Scaling = scaling;
		swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		ComPtr<IDXGISwapChain1> intermediateObject;
		auto result = device->GetFactory()->CreateSwapChainForCoreWindow(
			device->GetCommandQueue(),
			(IUnknown*)window,
			&swapchainDesc,
			nullptr,
			&intermediateObject
		);
		OSK_ASSERT2(IsDxSuccess(result), "Error al crear el swapchain.", result);

		swapchain.As(&intermediateObject);
	}

	void CreateImages(Device* device, MemoryAllocator* memory) {
		uint32_t currentFrame = swapchain->GetCurrentBackBufferIndex();
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE imageDescriptor(memory->GetRenderTargetDescriptorMemory()->GetCPUDescriptorHandleForHeapStart());
		for (uint32_t n = 0; n < NUMBER_OF_FRAMES; n++) {
			auto result = swapchain->GetBuffer(n, IID_PPV_ARGS(&images[n]));
			OSK_ASSERT2(IsDxSuccess(result), "Error al obtener imagen del swapchain.", result);

			device->GetDevice()->CreateRenderTargetView(images[n].Get(), nullptr, imageDescriptor);
			imageDescriptor.Offset(memory->GetRenderTargetDescriptorMemorySize());

			WCHAR name[25];
			if (swprintf_s(name, L"images[%u]", n) > 0)
				images[n]->SetName(name);
		}
	}

	void Recreate(uint32_t sizeX, uint32_t sizeY) {
		auto result = swapchain->ResizeBuffers(NUMBER_OF_FRAMES, sizeX, sizeY, format, 0);

		if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET)
			return;

		OSK_ASSERT2(IsDxSuccess(result), "Error al cambiar de tamaño el swapchain.", result);
	}

	ComPtr<IDXGISwapChain3> swapchain;
	ComPtr<ID3D12Resource> images[NUMBER_OF_FRAMES];

	DXGI_FORMAT format;

};
