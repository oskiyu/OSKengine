#include "CommandQueueDx12.h"

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include <wrl.h>
#include <d3d12.h>

#include "GpuDx12.h"
#include "GpuQueueTypes.h"

using namespace OSK;
using namespace OSK::GRAPHICS;
using namespace Microsoft::WRL;

CommandQueueDx12::CommandQueueDx12(GpuDx12* device, GpuQueueType type) : ICommandQueue(type) {
    D3D12_COMMAND_LIST_TYPE dxtype = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (type == GpuQueueType::ASYNC_TRANSFER) {
        dxtype = D3D12_COMMAND_LIST_TYPE_COPY;
    }

    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = dxtype;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    device->GetDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
}

ID3D12CommandQueue* CommandQueueDx12::GetCommandQueue() {
	return m_commandQueue.Get();
}

#endif
