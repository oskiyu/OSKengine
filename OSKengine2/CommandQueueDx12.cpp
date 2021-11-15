#include "CommandQueueDx12.h"

#include <wrl.h>
#include <d3d12.h>

using namespace OSK;
using namespace Microsoft::WRL;

void CommandQueueDx12::SetCommandQueue(const ComPtr<ID3D12CommandQueue>& commandQueue) {
	this->commandQueue = ComPtr<ID3D12CommandQueue>(commandQueue);
}

ID3D12CommandQueue* CommandQueueDx12::GetCommandQueue() const {
	return commandQueue.Get();
}
