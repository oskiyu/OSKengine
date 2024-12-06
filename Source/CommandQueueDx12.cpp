#include "CommandQueueDx12.h"

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include <wrl.h>
#include <d3d12.h>

using namespace OSK;
using namespace OSK::GRAPHICS;
using namespace Microsoft::WRL;

CommandQueueDx12::CommandQueueDx12(const QueueFamily& family, UIndex32 indexInsideFamily, GpuQueueType type) : ICommandQueue(family, type, indexInsideFamily) {

}

void CommandQueueDx12::SetCommandQueue(const ComPtr<ID3D12CommandQueue>& commandQueue) {
	this->commandQueue = ComPtr<ID3D12CommandQueue>(commandQueue);
}

ID3D12CommandQueue* CommandQueueDx12::GetCommandQueue() const {
	return commandQueue.Get();
}

#endif
