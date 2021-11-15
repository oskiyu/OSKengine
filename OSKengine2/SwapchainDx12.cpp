#include "SwapchainDx12.h"

#include "CommandQueueDx12.h"
#include "Window.h"
#include "FormatDx12.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

using namespace OSK;

void SwapchainDx12::Create(Format format, const CommandQueueDx12& commandQueue, IDXGIFactory4* factory, const Window& window) {
    DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

    swapchainDesc.BufferCount = 2;
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
}

IDXGISwapChain3* SwapchainDx12::GetSwapchain() const {
    return swapchain.Get();
}
