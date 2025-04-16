#include "CommandPoolDx12.h"

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "CommandListDx12.h"
#include "GpuDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


CommandPoolDx12::CommandPoolDx12(GpuDx12* device, CommandsSupport supportedCommands, GpuQueueType type) : ICommandPool(supportedCommands, type) {
	if (supportedCommands == CommandsSupport::TRANSFER) {
		m_type = D3D12_COMMAND_LIST_TYPE_COPY;
	}

	device->GetDevice()->CreateCommandAllocator(m_type, IID_PPV_ARGS(&m_commandPool));
}

UniquePtr<ICommandList> CommandPoolDx12::CreateCommandList(const IGpu& device) {
    return MakeUnique<CommandListDx12>(*device.As<GpuDx12>(), this);
}

UniquePtr<ICommandList> CommandPoolDx12::CreateSingleTimeCommandList(const IGpu& device) {
    return CreateCommandList(device);
}

ID3D12CommandAllocator* CommandPoolDx12::GetCommandAllocator() {
	return m_commandPool.Get();
}

D3D12_COMMAND_LIST_TYPE CommandPoolDx12::GetType() const {
	return m_type;
}

#endif
