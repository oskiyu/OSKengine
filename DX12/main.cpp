#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <wrl.h>

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

const uint8_t MAX_NUMBER_OF_FRAMES = 3;

HWND window;
//Tamaño antes de pantalla completa
RECT windowRectangle;

using Microsoft::WRL::ComPtr;

ComPtr<ID3D12Device2> logicalDevice;
ComPtr<ID3D12CommandQueue> commandBuffer;
ComPtr<IDXGISwapChain4> swapchain;

ComPtr<ID3D12Resource> swapchainImages[MAX_NUMBER_OF_FRAMES];

ComPtr<ID3D12CommandList> graphicsCommandBuffer;
ComPtr<ID3D12CommandAllocator> commandPool[MAX_NUMBER_OF_FRAMES];

ComPtr<ID3D12DescriptorHeap> descriptorPool;

uint32_t descriptorSize;
uint32_t currentImage = 0;