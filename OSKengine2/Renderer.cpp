#include "IRenderer.h"

#include "ICommandPool.h"
#include "IGpu.h"
#include "ISwapchain.h"
#include "MaterialSystem.h"
#include "IGpuImage.h"
#include "IGpuMemoryAllocator.h"
#include "IGpuDataBuffer.h"
#include "GpuImageLayout.h"
#include "ICommandList.h"

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

void IRenderer::UploadImageToGpu(GpuImage* destination, const TByte* data, TSize numBytes, GpuImageLayout finalLayout) {
	auto stagingBuffer = GetMemoryAllocator()->CreateStagingBuffer(numBytes);
	stagingBuffer->MapMemory();
	stagingBuffer->Write(data, numBytes);
	stagingBuffer->Unmap();

	auto commandList = CreateSingleUseCommandList();
	commandList->RegisterStagingBuffer(stagingBuffer);
	commandList->Reset();
	commandList->Start();
	commandList->TransitionImageLayout(destination, GpuImageLayout::TRANSFER_DESTINATION);
	commandList->CopyBufferToImage(stagingBuffer.GetPointer(), destination);
	commandList->TransitionImageLayout(destination, finalLayout);
	commandList->Close();
	SubmitSingleUseCommandList(commandList.GetPointer());
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
