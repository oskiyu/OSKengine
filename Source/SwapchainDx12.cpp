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

SwapchainDx12::SwapchainDx12(PresentMode mode, Format format, const GpuDx12& device, std::span<const UIndex32> queueIndices, const IDisplay& display, IDXGIFactory4* factory)
: ISwapchain(mode, format, queueIndices) 
{
    DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

    SetNumImagesInFlight(MAX_RESOURCES_IN_FLIGHT);

    swapchainDesc.BufferCount = GetImageCount();
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    swapchainDesc.Width = display.GetResolution().x;
    swapchainDesc.Height = display.GetResolution().y;
    swapchainDesc.Format = GetFormatDx12(format);

    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    swapchainDesc.SampleDesc.Count = 1;
    /*
    ComPtr<IDXGISwapChain1> swapchainTemp;
    factory->CreateSwapChainForHwnd(
        commandQueue.GetCommandQueue(), // swap chain forces flush when does flip
        glfwGetWin32Window(display.As<Window>()->_GetGlfw()),
        &swapchainDesc,
        nullptr, // pFullscreenDesc
        nullptr, // pRestrictToOutput
        &swapchainTemp
    );

    swapchainTemp.As(&swapchain);*/
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
    // ----------------- RENDER TARGETS ----------------- //
    D3D12_DESCRIPTOR_HEAP_DESC imagesMemoryCreateInfo{};
    imagesMemoryCreateInfo.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    imagesMemoryCreateInfo.NodeMask = 0;
    imagesMemoryCreateInfo.NumDescriptors = GetImageCount();
    imagesMemoryCreateInfo.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    D3D12_DESCRIPTOR_HEAP_DESC depthImagesMemoryCreateInfo{};
    depthImagesMemoryCreateInfo.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    depthImagesMemoryCreateInfo.NodeMask = 0;
    depthImagesMemoryCreateInfo.NumDescriptors = GetImageCount();
    depthImagesMemoryCreateInfo.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    /*m_device->As<GpuDx12>()->GetDevice()->CreateDescriptorHeap(&imagesMemoryCreateInfo, IID_PPV_ARGS(&renderTargetsDesc));
    auto result = m_device->As<GpuDx12>()->GetDevice()->CreateDescriptorHeap(&depthImagesMemoryCreateInfo, IID_PPV_ARGS(&depthTargetsDescHeap));

    const Vector3ui imageSize = Vector3ui(display.GetResolution().x, display.GetResolution().y, 1);
    for (UIndex32 i = 0; i < GetImageCount(); i++) {
        m_images[i] = new GpuImageDx12(imageSize, GpuImageDimension::d2D, GpuImageUsage::COLOR, 1, GetColorFormat(), 1, {});
        depthImages[i] = new GpuImageDx12(
            imageSize, 
            GpuImageDimension::d2D, 
            GpuImageUsage::DEPTH | GpuImageUsage::STENCIL, 
            1, m_colorFormat, 1, {});

        {
            ComPtr<ID3D12Resource> rTarget;
            swapchain->GetBuffer(i, IID_PPV_ARGS(&rTarget));

            m_images[i]->As<GpuImageDx12>()->_SetResource(rTarget);

            D3D12_RENDER_TARGET_VIEW_DESC renderTargetDesc{};
            renderTargetDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            renderTargetDesc.Format = GetFormatDx12(m_colorFormat);
            renderTargetDesc.Texture2D.MipSlice = 0;
            renderTargetDesc.Texture2D.PlaneSlice = 0;

            D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDescriptor = renderTargetsDesc->GetCPUDescriptorHandleForHeapStart();
            renderTargetDescriptor.ptr += ((SIZE_T)i) * m_device->As<GpuDx12>()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

            m_device->As<GpuDx12>()->GetDevice()->CreateRenderTargetView(m_images[i]->As<GpuImageDx12>()->GetResource(), &renderTargetDesc, renderTargetDescriptor);
        }

        // Depth

        {
            D3D12_RESOURCE_DESC depthResourceDesc{};
            depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            depthResourceDesc.Alignment = 0;
            depthResourceDesc.Width = depthImages[i]->GetSize2D().x;
            depthResourceDesc.Height = depthImages[i]->GetSize2D().y;
            depthResourceDesc.DepthOrArraySize = 1;
            depthResourceDesc.SampleDesc.Count = 1;
            depthResourceDesc.SampleDesc.Quality = 0;
            depthResourceDesc.MipLevels = 0;
            depthResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            depthResourceDesc.Format = GetFormatDx12(Format::D32S8_SFLOAT_SUINT);
            depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

            D3D12_CLEAR_VALUE depthClearValue = {};
            depthClearValue.Format = GetFormatDx12(Format::D32S8_SFLOAT_SUINT);
            depthClearValue.DepthStencil.Depth = 1.0f;
            depthClearValue.DepthStencil.Stencil = 0;

            ComPtr<ID3D12Resource> depthResource;
            D3D12_RESOURCE_ALLOCATION_INFO allocInfo = Engine::GetRenderer()->GetGpu()->As<GpuDx12>()->GetDevice()->GetResourceAllocationInfo(0, 1, &depthResourceDesc);

            D3D12_HEAP_DESC memoryCreateInfo{};
            memoryCreateInfo.SizeInBytes = allocInfo.SizeInBytes;
            memoryCreateInfo.Properties.Type = GetGpuSharedMemoryTypeDx12(GpuSharedMemoryType::GPU_ONLY);

            m_device->As<GpuDx12>()->GetDevice()->CreateHeap(&memoryCreateInfo, IID_PPV_ARGS(&depthHeaps[i]));
            m_device->As<GpuDx12>()->GetDevice()->CreatePlacedResource(
                depthHeaps[i].Get(), 0, &depthResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &depthClearValue, IID_PPV_ARGS(&depthResource));
            depthImages[i]->As<GpuImageDx12>()->_SetResource(depthResource);

            D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
            depthStencilViewDesc.Format = GetFormatDx12(Format::D32S8_SFLOAT_SUINT);
            depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

            D3D12_CPU_DESCRIPTOR_HANDLE depthDescriptor = depthTargetsDescHeap->GetCPUDescriptorHandleForHeapStart();
            depthDescriptor.ptr += ((SIZE_T)i) * m_device->As<GpuDx12>()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

            m_device->As<GpuDx12>()->GetDevice()->CreateDepthStencilView(depthImages[i]->As<GpuImageDx12>()->GetResource(), &depthStencilViewDesc, depthDescriptor);
        }
    }*/
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
