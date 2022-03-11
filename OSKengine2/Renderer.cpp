#include "IRenderer.h"

#include "ICommandPool.h"
#include "IGpu.h"
#include "ISwapchain.h"
#include "MaterialSystem.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IRenderer::IRenderer(RenderApiType type) : renderApiType(type) {
	materialSystem = new MaterialSystem;
}

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

MaterialSystem* IRenderer::GetMaterialSystem() const {
	return materialSystem.GetPointer();
}

RenderApiType IRenderer::GetRenderApi() const {
	return renderApiType;
}

TSize IRenderer::GetSwapchainImagesCount() const {
	return swapchain->GetImageCount();
}

bool IRenderer::IsOpen() const {
	return isOpen;
}
