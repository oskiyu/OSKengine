#include "SwapchainDx12.h"

#include "CommandQueueDx12.h"
#include "Window.h"
#include "FormatDx12.h"
#include "GpuDx12.h"
#include "IGpuMemorySubblock.h"
#include "GpuMemorySubblockDx12.h"
#include "GpuMemoryBlockDx12.h"
#include "GpuMemoryBlockDx12.h"
#include "OSKengine.h"
#include "RendererDx12.h"
#include "IGpuMemoryAllocator.h"
#include "GpuMemoryAllocatorDx12.h"
#include "IGpuImage.h"
#include "GpuImageDx12.h"

#include "OSKengine.h"
#include "Logger.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::GRAPHICS;

SwapchainDx12::~SwapchainDx12() {
    if (swapchain.Get())
        swapchain.Reset();

    Engine::GetLogger()->InfoLog("Swapchain cerrado.");
}

void SwapchainDx12::Create(IGpu* device, Format format, const CommandQueueDx12& commandQueue, IDXGIFactory4* factory, const IO::Window& window) {
    this->format = format;
    this->device = device;

    DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

    swapchainDesc.BufferCount = imageCount;
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    swapchainDesc.Width = window.GetWindowSize().X;
    swapchainDesc.Height = window.GetWindowSize().Y;
    swapchainDesc.Format = GetFormatDx12(format);

    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    swapchainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapchainTemp;
    factory->CreateSwapChainForHwnd(
        commandQueue.GetCommandQueue(), // swap chain forces flush when does flip
        glfwGetWin32Window(window._GetGlfw()),
        &swapchainDesc,
        nullptr, // pFullscreenDesc
        nullptr, // pRestrictToOutput
        &swapchainTemp
    );

    swapchainTemp.As(&swapchain);

    // ----------------- RENDER TARGETS ----------------- //
    D3D12_DESCRIPTOR_HEAP_DESC imagesMemoryCreateInfo{};
    imagesMemoryCreateInfo.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    imagesMemoryCreateInfo.NodeMask = 0;
    imagesMemoryCreateInfo.NumDescriptors = imageCount;
    imagesMemoryCreateInfo.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    device->As<GpuDx12>()->GetDevice()->CreateDescriptorHeap(&imagesMemoryCreateInfo, IID_PPV_ARGS(&renderTargetsDesc));

    CreateImages(window);
}

void SwapchainDx12::DeleteImages() {
    for (TSize i = 0; i < imageCount; i++)
        images[i].Release();
}

void SwapchainDx12::CreateImages(const IO::Window& window) {
    for (TSize i = 0; i < imageCount; i++) {
        images[i] = new GpuImageDx12(window.GetWindowSize().X, window.GetWindowSize().Y, format);

        ComPtr<ID3D12Resource> rTarget;
        swapchain->GetBuffer(i, IID_PPV_ARGS(&rTarget));

        images[i]->As<GpuImageDx12>()->SetResource(rTarget);

        D3D12_RENDER_TARGET_VIEW_DESC RTDesc{};
        RTDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        RTDesc.Format = GetFormatDx12(format);
        RTDesc.Texture2D.MipSlice = 0;
        RTDesc.Texture2D.PlaneSlice = 0;

        D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor = renderTargetsDesc->GetCPUDescriptorHandleForHeapStart();
        DestDescriptor.ptr += ((SIZE_T)i) * device->As<GpuDx12>()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        device->As<GpuDx12>()->GetDevice()->CreateRenderTargetView(images[i]->As<GpuImageDx12>()->GetResource(), &RTDesc, DestDescriptor);
    }
}

void SwapchainDx12::Present() {
    swapchain->Present(1, 0);

    currentFrameIndex = swapchain->GetCurrentBackBufferIndex();
}

IDXGISwapChain3* SwapchainDx12::GetSwapchain() const {
    return swapchain.Get();
}

ID3D12DescriptorHeap* SwapchainDx12::GetRenderTargetMemory() const {
    return this->renderTargetsDesc.Get();
}
