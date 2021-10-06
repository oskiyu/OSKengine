#include "Device.h"
#include "Window.h"
#include "Fence.h"
#include "Swapchain.h"

//Sync
ComPtr<ID3D12Fence> nFence;
uint64_t fenceValue = 0;
uint64_t frameFenceValues[NUMBER_OF_FRAMES] = {};
HANDLE fenceEvent;

Window window;
Device device;
Swapchain swapchain;
Fence fence;
MemoryAllocator allocator;

uint32_t currentFrame = 0;

bool hasBeenInit = false;

void Update() {
    /*static uint64_t frameCounter = 0;
    static double elapsedSeconds = 0.0;
    static std::chrono::high_resolution_clock clock;
    static auto t0 = clock.now();

    frameCounter++;
    auto t1 = clock.now();
    auto deltaTime = t1 - t0;
    t0 = t1;

    elapsedSeconds += deltaTime.count() * 1e-9;
    if (elapsedSeconds > 1.0) {
        char buffer[500];
        auto fps = frameCounter / elapsedSeconds;
        sprintf_s(buffer, 500, "FPS: %f\n", fps);
        OutputDebugString(buffer);

        frameCounter = 0;
        elapsedSeconds = 0.0;
    }*/
}

void Render() {
    if (!hasBeenInit)
        return;

    device.GetCommandPool(currentFrame)->Reset();
    device.GetGraphicsCommandQueue()->Reset(device.GetCommandPool(currentFrame), nullptr);

    auto targetBackbuffer = swapchain.GetBackbufferImage(currentFrame);

    CD3DX12_RESOURCE_BARRIER toRendertargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        targetBackbuffer,
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    device.GetGraphicsCommandQueue()->ResourceBarrier(1, &toRendertargetBarrier);

    const FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
        allocator.GetRenderTargetDescriptorMemory()->GetCPUDescriptorHandleForHeapStart(),
        currentFrame, 
        allocator.GetRenderTargetDescriptorMemorySize()
    );
    device.GetGraphicsCommandQueue()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

    CD3DX12_RESOURCE_BARRIER toPresentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        targetBackbuffer,
        D3D12_RESOURCE_STATE_RENDER_TARGET, 
        D3D12_RESOURCE_STATE_PRESENT
    );
    device.GetGraphicsCommandQueue()->ResourceBarrier(1, &toPresentBarrier);

    auto result = device.GetGraphicsCommandQueue()->Close();
    OSK_ASSERT2(IsDxSuccess(result), "Error al cerrar el command queue.", result);

    ID3D12CommandList* const commandLists[] = { device.GetGraphicsCommandQueue()  };
    device.GetCommandQueue()->ExecuteCommandLists(1, commandLists);

    fence.perFrameValues[currentFrame] = fence.Signal(device.GetCommandQueue(), fence.value);

    swapchain.Present();

    currentFrame = swapchain.GetCurrentBackbufferIndex();
    fence.Wait(fence.perFrameValues[currentFrame]);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdline, int cmdshow) {
    Window window;
    window.Create(hInst, hPrevInst, cmdline, cmdshow);

    Device logicalDevice;
    logicalDevice.Create();

    MemoryAllocator allocator;
    allocator.Create(&logicalDevice);

    Swapchain swapchain;
    swapchain.Create(&logicalDevice, window.GetWindowHandle(), window.GetSizeX(), window.GetSizeY(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

    window.Run();

    return 0;
}

void Resize(uint32_t width, uint32_t height) {
    if (window.GetSizeX() != width || window.GetSizeY() != height) {
        //window.SetSize(max(1u, width), max(1u, height));

        fence.Flush(device.GetCommandQueue(), fence.value);

        for (int i = 0; i < NUMBER_OF_FRAMES; i++) {
            swapchain.ResetBackbufferImage(i);

            fence.perFrameValues[i] = fence.perFrameValues[currentFrame];
        }

        swapchain.Recreate(width, height);

        currentFrame = swapchain.GetCurrentBackbufferIndex();
    }
}


LRESULT CALLBACK WindowCallback(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (!hasBeenInit)
        return DefWindowProc(hwnd, msg, wp, lp);

    if (msg == WM_PAINT) {
        Render();
    }
    else if (msg == WM_SIZE) {
        RECT clientRect = {};
        ::GetClientRect(hwnd, &clientRect);

        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;

        Resize(width, height);
    }
    else if (msg == WM_DESTROY)
        PostQuitMessage(0);

    return DefWindowProc(hwnd, msg, wp, lp);
}
