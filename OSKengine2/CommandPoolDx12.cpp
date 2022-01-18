#include "CommandPoolDx12.h"

#include "CommandListDx12.h"
#include "GpuDx12.h"

using namespace OSK;

OwnedPtr<ICommandList> CommandPoolDx12::CreateCommandList(const IGpu& device) {
	auto output = new CommandListDx12;

    ComPtr<ID3D12GraphicsCommandList> list;

    device.As<GpuDx12>()->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
        commandPool.Get(), nullptr, IID_PPV_ARGS(&list));
    list->Close();

    output->SetCommandList(list);
    output->SetCommandPool(*this);

    return output;
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
