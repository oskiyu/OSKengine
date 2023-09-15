#include "IRenderer.h"

#include "ICommandPool.h"
#include "IGpu.h"
#include "ISwapchain.h"
#include "MaterialSystem.h"
#include "IGpuImage.h"
#include "IGpuMemoryAllocator.h"
#include "GpuBuffer.h"
#include "GpuImageLayout.h"
#include "ICommandList.h"
#include "Window.h"
#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "IRenderSystem.h"
#include "Format.h"
#include "IPostProcessPass.h"

#include "InvalidArgumentException.h"

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

IRenderer::IRenderer(RenderApiType type, bool requestRayTracing) : renderApiType(type), isRtRequested(requestRayTracing) {
	materialSystem = new MaterialSystem;
}

ICommandList* IRenderer::GetPreComputeCommandList() {
	return singleCommandQueue ? graphicsCommandList.GetPointer() : preComputeCommandList.GetPointer();
}

ICommandList* IRenderer::GetGraphicsCommandList() {
	return graphicsCommandList.GetPointer();
}

ICommandList* IRenderer::GetPostComputeCommandList() {
	return singleCommandQueue ? graphicsCommandList.GetPointer() : postComputeCommandList.GetPointer();
}

ICommandList* IRenderer::GetFrameBuildCommandList() {
	return singleCommandQueue ? graphicsCommandList.GetPointer() : frameBuildCommandList.GetPointer();
}

IGpuMemoryAllocator* IRenderer::GetAllocator() {
	return gpuMemoryAllocator.GetPointer();
}

IGpu* IRenderer::GetGpu() {
	return currentGpu.GetPointer();
}

void IRenderer::UploadLayeredImageToGpu(GpuImage* destination, const TByte* data, USize64 numBytes, USize32 numLayers, ICommandList* cmdList) {
	OSK_ASSERT(numLayers > 0, InvalidArgumentException("El número de capas debe ser > 0."));
	OSK_ASSERT(numBytes > 0, InvalidArgumentException("El número de bytes debe ser > 0."));

	USize64 gpuNumBytes = destination->GetPhysicalNumberOfBytes() * numLayers;

	const TByte* uploadableData = data;

	auto stagingBuffer = GetAllocator()->CreateStagingBuffer(gpuNumBytes);
	stagingBuffer->MapMemory();
	stagingBuffer->Write(uploadableData, gpuNumBytes);
	stagingBuffer->Unmap();

	USize64 offsetPerIteration = gpuNumBytes / numLayers;
	for (UIndex32 i = 0; i < numLayers; i++)
		cmdList->CopyBufferToImage(stagingBuffer.GetValue(), destination, i, offsetPerIteration * i);

	cmdList->RegisterStagingBuffer(stagingBuffer);
}

void IRenderer::SetPresentMode(PresentMode mode) {
	swapchain->SetPresentMode(mode);
}

void IRenderer::HandleResize() {
	const Vector2ui windowSize = display->GetResolution();

	renderTargetsCamera->UpdateUniformBuffer(renderTargetsCameraTransform);

	if (Engine::GetEcs())
		for (auto i : Engine::GetEcs()->GetRenderSystems())
			i->Resize(windowSize);

	finalRenderTarget->Resize(windowSize);

	Engine::GetEcs()->PublishEvent<IDisplay::ResolutionChangedEvent>({ windowSize });
}

const TByte* IRenderer::FormatImageDataForGpu(const GpuImage* image, const TByte* data, USize32 numLayers) {
	return data;
}

void IRenderer::UploadImageToGpu(GpuImage* destination, const TByte* data, USize64 numBytes, ICommandList* cmdList) {
	UploadLayeredImageToGpu(destination, data, numBytes, 1, cmdList);
}

void IRenderer::UploadCubemapImageToGpu(GpuImage* destination, const TByte* data, USize64 numBytes, ICommandList* cmdList) {
	UploadLayeredImageToGpu(destination, data, numBytes, 6, cmdList);
}

OwnedPtr<ICommandList> IRenderer::CreateSingleUseCommandList() {
	auto output = graphicsCommandPool->CreateCommandList(currentGpu.GetValue());
	output->_SetSingleTimeUse();

	return output;
}

MaterialSystem* IRenderer::GetMaterialSystem() {
	return materialSystem.GetPointer();
}

void IRenderer::CreateMainRenderpass() {
	RenderTargetAttachmentInfo colorInfo{ .format = swapchain->GetColorFormat(), .name = "Final Color Target" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D32S8_SFLOAT_SUINT, .name = "Final Depth Target" };
	finalRenderTarget = new RenderTarget;
	finalRenderTarget->CreateAsFinal(swapchain->GetImage(0)->GetSize2D(),
		{ colorInfo }, depthInfo);
}

RenderApiType IRenderer::GetRenderApi() const {
	return renderApiType;
}

USize32 IRenderer::GetSwapchainImagesCount() const {
	return swapchain->GetImageCount();
}

bool IRenderer::IsOpen() const {
	return isOpen;
}

RenderTarget* IRenderer::GetFinalRenderTarget() {
	return finalRenderTarget.GetPointer();
}

UIndex32 IRenderer::GetCurrentResourceIndex() const {
	return this->GetCurrentCommandListIndex();
}

const ECS::CameraComponent2D& IRenderer::GetRenderTargetsCamera() const {
	return *renderTargetsCamera.GetPointer();
}

ISwapchain* IRenderer::_GetSwapchain() {
	return swapchain.GetPointer();
}

bool IRenderer::_HasImplicitResizeHandling() const {
	return implicitResizeHandling;
}

Material* IRenderer::GetFullscreenRenderingMaterial() {
	return materialSystem->LoadMaterial("Resources/Materials/2D/material_rendertarget.json");
}

bool IRenderer::IsRtActive() const {
	return isRtActive;
}

bool IRenderer::IsRtRequested() const {
	return isRtRequested;
}
