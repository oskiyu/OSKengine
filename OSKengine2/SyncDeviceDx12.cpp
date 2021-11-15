#include "SyncDeviceDx12.h"

#include "GpuDx12.h"
#include "CommandQueueDx12.h"

using namespace OSK;

void SyncDeviceDx12::SetFence(const ComPtr<ID3D12Fence>& fence) {
	this->fence = fence;
}

void SyncDeviceDx12::SetFenceEvent(HANDLE fenceEvent) {
	this->fenceEvent = fenceEvent;
}

void SyncDeviceDx12::Await() {
	if (fence->GetCompletedValue() < previousFenceValue) {
		fence->SetEventOnCompletion(previousFenceValue, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void SyncDeviceDx12::Flush(const CommandQueueDx12& commandQueue) {
	previousFenceValue = fenceValue;
	commandQueue.GetCommandQueue()->Signal(fence.Get(), previousFenceValue);

	fenceValue++;
}
