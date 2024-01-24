#include "GpuDx12.h"

#include "CommandPoolDx12.h"

#include "Assert.h"
#include "NotImplementedException.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuDx12::~GpuDx12() {
	Close();
}

void GpuDx12::Close() {

}

GpuMemoryUsageInfo GpuDx12::GetMemoryUsageInfo() const {
	OSK_ASSERT(false, NotImplementedException());
	return {};
}

OwnedPtr<ICommandPool> GpuDx12::CreateGraphicsCommandPool() {
	auto output = new CommandPoolDx12;

	ComPtr<ID3D12CommandAllocator> commandAllocator;
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));

	output->SetCommandPool(commandAllocator);

	return output;
}

OwnedPtr<ICommandPool> GpuDx12::CreateComputeCommandPool() {
	auto output = new CommandPoolDx12;

	ComPtr<ID3D12CommandAllocator> commandAllocator;
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&commandAllocator));

	output->SetCommandPool(commandAllocator);

	return output;
}

void GpuDx12::SetAdapter(const ComPtr<IDXGIAdapter1>& adapter) {
	this->adapter = adapter;
}

void GpuDx12::SetDevice(const ComPtr<ID3D12Device>& device) {
	this->device = device;
}

IDXGIAdapter1* GpuDx12::GetAdapter() const {
	return adapter.Get();
}

ID3D12Device* GpuDx12::GetDevice() const {
	return this->device.Get();
}
