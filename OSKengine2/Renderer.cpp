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
#include "Window.h"
#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "IRenderSystem.h"
#include "Format.h"

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ECS;
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

void IRenderer::UploadLayeredImageToGpu(GpuImage* destination, const TByte* data, TSize numBytes, GpuImageLayout finalLayout, TSize numLayers) {
	TSize gpuNumBytes = destination->GetPhysicalNumberOfBytes() * numLayers;

	const TByte* uploadableData = this->FormatImageDataForGpu(destination, data, numLayers);

	auto stagingBuffer = GetMemoryAllocator()->CreateStagingBuffer(gpuNumBytes);
	stagingBuffer->MapMemory();
	stagingBuffer->Write(uploadableData, gpuNumBytes);
	stagingBuffer->Unmap();

	auto commandList = CreateSingleUseCommandList();
	commandList->RegisterStagingBuffer(stagingBuffer);
	commandList->Reset();
	commandList->Start();
	commandList->TransitionImageLayout(destination, GpuImageLayout::TRANSFER_DESTINATION, 0, numLayers);

	TSize offsetPerIteration = gpuNumBytes / numLayers;
	for (TSize i = 0; i < numLayers; i++)
		commandList->CopyBufferToImage(stagingBuffer.GetPointer(), destination, i, offsetPerIteration * i);

	commandList->TransitionImageLayout(destination, finalLayout, 0, numLayers);

	commandList->Close();
	SubmitSingleUseCommandList(commandList.GetPointer());

	if (data != uploadableData)
		delete[] uploadableData;
}

void IRenderer::HandleResize() {
	const Vector2ui windowSize = window->GetWindowSize();

	renderTargetsCamera->UpdateUniformBuffer(renderTargetsCameraTransform);

	if (Engine::GetEntityComponentSystem())
		for (auto i : Engine::GetEntityComponentSystem()->GetRenderSystems())
			i->Resize(windowSize);

	for (auto i : resizableRenderTargets)
		i->Resize(windowSize);

	finalRenderTarget->Resize(windowSize);
}

const TByte* IRenderer::FormatImageDataForGpu(const GpuImage* image, const TByte* data, TSize numLayers) {
	return data;
}

void IRenderer::UploadImageToGpu(GpuImage* destination, const TByte* data, TSize numBytes, GpuImageLayout finalLayout) {
	UploadLayeredImageToGpu(destination, data, numBytes, finalLayout, 1);
}

void IRenderer::UploadCubemapImageToGpu(GpuImage* destination, const TByte* data, TSize numBytes, GpuImageLayout finalLayout) {
	UploadLayeredImageToGpu(destination, data, numBytes, finalLayout, 6);
}

OwnedPtr<ICommandList> IRenderer::CreateSingleUseCommandList() {
	auto output = commandPool->CreateCommandList(currentGpu.GetValue());
	output->_SetSingleTimeUse();

	return output;
}

MaterialSystem* IRenderer::GetMaterialSystem() const {
	return materialSystem.GetPointer();
}

void IRenderer::CreateMainRenderpass() {
	finalRenderTarget = new RenderTarget;
	finalRenderTarget->SetRenderTargetType(RenderpassType::FINAL);
	finalRenderTarget->Create({ swapchain->GetImage(0)->GetSize().X, swapchain->GetImage(0)->GetSize().Y },
		swapchain->GetColorFormat(), Format::D32S8_SFLOAT_SUINT);
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

RenderTarget* IRenderer::GetFinalRenderTarget() const {
	return finalRenderTarget.GetPointer();
}

const ECS::CameraComponent2D& IRenderer::GetRenderTargetsCamera() const {
	return *renderTargetsCamera.GetPointer();
}

ISwapchain* IRenderer::_GetSwapchain() const {
	return swapchain.GetPointer();
}

bool IRenderer::_HasImplicitResizeHandling() const {
	return implicitResizeHandling;
}

void IRenderer::RegisterRenderTarget(RenderTarget* renderTarget) {
	if (!resizableRenderTargets.ContainsElement(renderTarget))
		resizableRenderTargets.Insert(renderTarget);
}

void IRenderer::UnregisterRenderTarget(RenderTarget* renderTarget) {
	resizableRenderTargets.Remove(renderTarget);
}
