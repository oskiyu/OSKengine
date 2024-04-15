#include "CommandPoolDx12.h"

#include "CommandListDx12.h"
#include "GpuDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


CommandPoolDx12::CommandPoolDx12(CommandsSupport supportedCommands, GpuQueueType type) : ICommandPool(supportedCommands, type) {

}

OwnedPtr<ICommandList> CommandPoolDx12::CreateCommandList(const IGpu& device) {
    return new CommandListDx12(*device.As<GpuDx12>(), *this);
}

OwnedPtr<ICommandList> CommandPoolDx12::CreateSingleTimeCommandList(const IGpu& device) {
    return CreateCommandList(device);
}

void CommandPoolDx12::SetCommandPool(const ComPtr<ID3D12CommandAllocator>& commandPool) {
	this->commandPool = commandPool;
}

ID3D12CommandAllocator* CommandPoolDx12::GetCommandAllocator() const {
	return commandPool.Get();
}
