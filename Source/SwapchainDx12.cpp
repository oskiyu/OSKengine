#include "SwapchainDx12.h"

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

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
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"

#include "OSKengine.h"
#include "Logger.h"

#include <d3d12.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "GpuMemoryTypesDx12.h"
#include "PresentMode.h"

#ifdef max
#undef max
#endif // max


using namespace OSK;
using namespace OSK::IO;
using namespace OSK::GRAPHICS;


DXGI_SWAP_EFFECT GetPresentModeDx12(PresentMode mode) {
    switch (mode) {
    case OSK::GRAPHICS::PresentMode::VSYNC_OFF:
        return DXGI_SWAP_EFFECT_DISCARD;
    case OSK::GRAPHICS::PresentMode::VSYNC_ON:
    case OSK::GRAPHICS::PresentMode::VSYNC_ON_TRIPLE_BUFFER:
        return DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    }

    return DXGI_SWAP_EFFECT_DISCARD;
}

SwapchainDx12::SwapchainDx12(CommandQueueDx12* queue, PresentMode mode, Format format, GpuDx12* device, IDisplay& display, IDXGIFactory4* factory)
: ISwapchain(mode, format), m_device(device), m_queue(queue)
{
    SetNumImagesInFlight(MAX_RESOURCES_IN_FLIGHT);

    DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

    swapchainDesc.BufferCount = GetImageCount();
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    swapchainDesc.Width = display.GetResolution().x;
    swapchainDesc.Height = display.GetResolution().y;
    swapchainDesc.Format = GetFormatDx12(format);

    swapchainDesc.SwapEffect = GetPresentModeDx12(mode);

    swapchainDesc.SampleDesc.Count = 1;
    
    ComPtr<IDXGISwapChain1> swapchainTemp;
    factory->CreateSwapChainForHwnd(
        queue->GetCommandQueue(),
        glfwGetWin32Window(display.As<Window>()->_GetGlfw()),
        &swapchainDesc,
        nullptr, // pFullscreenDesc
        nullptr, // pRestrictToOutput
        &swapchainTemp);

    swapchainTemp.As(&swapchain);

    CreateImages(display);
}

SwapchainDx12::~SwapchainDx12() {
    if (swapchain.Get())
        swapchain.Reset();
}


void SwapchainDx12::DeleteImages() {
    // for (USize32 i = 0; i < m_imageCount; i++)
    //    m_images[i].Delete();

    renderTargetsDesc->Release();
}

void SwapchainDx12::CreateImages(const IO::IDisplay& display) {
    D3D12_DESCRIPTOR_HEAP_DESC imagesMemoryCreateInfo{};
    imagesMemoryCreateInfo.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    imagesMemoryCreateInfo.NodeMask = 0;
    imagesMemoryCreateInfo.NumDescriptors = GetImageCount();
    imagesMemoryCreateInfo.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    auto result = m_device->GetDevice()->CreateDescriptorHeap(&imagesMemoryCreateInfo, IID_PPV_ARGS(&renderTargetsDesc));

    const Vector3ui imageSize = Vector3ui(display.GetResolution().x, display.GetResolution().y, 1);

    for (UIndex32 i = 0; i < GetImageCount(); i++) {
        GpuImageCreateInfo desc = GpuImageCreateInfo::CreateDefault3D(imageSize, GetColorFormat(), GpuImageUsage::COLOR);
        desc.dimension = GpuImageDimension::d2D;
        SetImage(MakeUnique<GpuImageDx12>(desc, m_queue), i);

        ComPtr<ID3D12Resource> rTarget;
        swapchain->GetBuffer(i, IID_PPV_ARGS(&rTarget));

        auto* imageDx12 = GetImage(i)->As<GpuImageDx12>();
        imageDx12->_SetResource(rTarget);

        D3D12_RENDER_TARGET_VIEW_DESC renderTargetDesc{};
        renderTargetDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        renderTargetDesc.Format = GetFormatDx12(GetColorFormat());
        renderTargetDesc.Texture2D.MipSlice = 0;
        renderTargetDesc.Texture2D.PlaneSlice = 0;

        D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDescriptor = renderTargetsDesc->GetCPUDescriptorHandleForHeapStart();
        renderTargetDescriptor.ptr += ((SIZE_T)i) * m_device->As<GpuDx12>()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        m_device->As<GpuDx12>()->GetDevice()->CreateRenderTargetView(imageDx12->GetResource(), &renderTargetDesc, renderTargetDescriptor);
    }
}

void SwapchainDx12::Present() {
    swapchain->Present(static_cast<UINT>(GetCurrentPresentMode()), 0);
    UpdateFrameIndex();
}

void SwapchainDx12::UpdateFrameIndex() {
    // m_currentFrameIndex = swapchain->GetCurrentBackBufferIndex();
}

IDXGISwapChain3* SwapchainDx12::GetSwapchain() const {
    return swapchain.Get();
}

ID3D12DescriptorHeap* SwapchainDx12::GetRenderTargetMemory() const {
    return this->renderTargetsDesc.Get();
}

ID3D12DescriptorHeap* SwapchainDx12::GetDepthStencilMemory() const {
    return this->depthTargetsDescHeap.Get();
}

void SwapchainDx12::Resize(const IGpu& gpu, Vector2ui newResolution) {
    /// @todo
}

#endif
