#include "IRenderer.h"

#include "ICommandPool.h"
#include "IGpu.h"

using namespace OSK;

ICommandList* IRenderer::GetCommandList() const {
	return commandList.GetPointer();
}

IGpuMemoryAllocator* IRenderer::GetMemoryAllocator() const {
	return gpuMemoryAllocator.GetPointer();
}

IGpu* IRenderer::GetGpu() const {
	return currentGpu.GetPointer();
}

OwnedPtr<ICommandList> IRenderer::CreateSingleUseCommandList() {
	return commandPool->CreateCommandList(currentGpu.GetValue());
}
