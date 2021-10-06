#pragma once

#include "DX12.h"
#include "Device.h"
#include "MemoryAllocator.h"

class Swapchain {

public:

	void Create(Device* device, HWND window, uint32_t sizeX, uint32_t sizeY, DXGI_FORMAT format) {
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
		swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		ComPtr<IDXGISwapChain1> intermediateObject;
		auto result = device->GetFactory()->CreateSwapChainForHwnd(
			device->GetCommandQueue(),
			window,
			&swapchainDesc,
			nullptr,
			nullptr,
			&intermediateObject
		);
		OSK_ASSERT2(IsDxSuccess(result), "Error al crear el swapchain.", result);

		result = intermediateObject.As(&swapchain); 
		OSK_ASSERT2(IsDxSuccess(result), "Error al transformar el swapchain.", result);
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

		OSK_ASSERT2(IsDxSuccess(result), "Error al cambiar de tama�o el swapchain.", result);
	}

	void Present() {
		swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
	}

	uint32_t GetCurrentBackbufferIndex() const {
		return swapchain->GetCurrentBackBufferIndex();
	}

	ID3D12Resource* GetBackbufferImage(uint32_t index) const {
		return images[index].Get();
	}

	void ResetBackbufferImage(uint32_t index) {
		images[index].Reset();
	}

private:

	ComPtr<IDXGISwapChain4> swapchain;
	ComPtr<ID3D12Resource> images[NUMBER_OF_FRAMES];

	DXGI_FORMAT format;

};
