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

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

IRenderer::IRenderer(RenderApiType type, bool requestRayTracing) : renderApiType(type), isRtRequested(requestRayTracing) {
	materialSystem = new MaterialSystem;
}

ICommandList* IRenderer::GetPreComputeCommandList() const{
	return singleCommandQueue ? graphicsCommandList.GetPointer() : preComputeCommandList.GetPointer();
}

ICommandList* IRenderer::GetGraphicsCommandList() const {
	return graphicsCommandList.GetPointer();
}

ICommandList* IRenderer::GetPostComputeCommandList() const{
	return singleCommandQueue ? graphicsCommandList.GetPointer() : postComputeCommandList.GetPointer();
}

ICommandList* IRenderer::GetFrameBuildCommandList() const {
	return singleCommandQueue ? graphicsCommandList.GetPointer() : frameBuildCommandList.GetPointer();
}

IGpuMemoryAllocator* IRenderer::GetAllocator() const {
	return gpuMemoryAllocator.GetPointer();
}

IGpu* IRenderer::GetGpu() const {
	return currentGpu.GetPointer();
}

void IRenderer::UploadLayeredImageToGpu(GpuImage* destination, const TByte* data, TSize numBytes, TSize numLayers, ICommandList* cmdList) {
	OSK_ASSERT(numLayers > 0, "El número de capas debe ser > 0.");
	OSK_ASSUME(numLayers > 0);

	OSK_ASSERT(numBytes > 0, "El número de bytes debe ser > 0.");
	OSK_ASSUME(numLayers > 0);

	TSize gpuNumBytes = destination->GetPhysicalNumberOfBytes() * numLayers;

	const TByte* uploadableData = data;

	auto stagingBuffer = GetAllocator()->CreateStagingBuffer(gpuNumBytes);
	stagingBuffer->MapMemory();
	stagingBuffer->Write(uploadableData, gpuNumBytes);
	stagingBuffer->Unmap();

	TSize offsetPerIteration = gpuNumBytes / numLayers;
	for (TSize i = 0; i < numLayers; i++)
		cmdList->CopyBufferToImage(stagingBuffer.GetValue(), destination, i, offsetPerIteration * i);

	cmdList->RegisterStagingBuffer(stagingBuffer);
}

void IRenderer::HandleResize() {
	const Vector2ui windowSize = display->GetResolution();

	renderTargetsCamera->UpdateUniformBuffer(renderTargetsCameraTransform);

	if (Engine::GetEcs())
		for (auto i : Engine::GetEcs()->GetRenderSystems())
			i->Resize(windowSize);

	for (auto i : resizableRenderTargets)
		i->Resize(windowSize);

	for (auto i : resizablePostProcessingPasses)
		i->Resize(windowSize);

	for (auto& i : resizeCallbacks)
		i(windowSize);

	finalRenderTarget->Resize(windowSize);
}

const TByte* IRenderer::FormatImageDataForGpu(const GpuImage* image, const TByte* data, TSize numLayers) {
	return data;
}

void IRenderer::UploadImageToGpu(GpuImage* destination, const TByte* data, TSize numBytes, ICommandList* cmdList) {
	UploadLayeredImageToGpu(destination, data, numBytes, 1, cmdList);
}

void IRenderer::UploadCubemapImageToGpu(GpuImage* destination, const TByte* data, TSize numBytes, ICommandList* cmdList) {
	UploadLayeredImageToGpu(destination, data, numBytes, 6, cmdList);
}

OwnedPtr<ICommandList> IRenderer::CreateSingleUseCommandList() {
	auto output = graphicsCommandPool->CreateCommandList(currentGpu.GetValue());
	output->_SetSingleTimeUse();

	return output;
}

MaterialSystem* IRenderer::GetMaterialSystem() const {
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

TSize IRenderer::GetSwapchainImagesCount() const {
	return swapchain->GetImageCount();
}

bool IRenderer::IsOpen() const {
	return isOpen;
}

RenderTarget* IRenderer::GetFinalRenderTarget() const {
	return finalRenderTarget.GetPointer();
}

TIndex IRenderer::GetCurrentResourceIndex() const {
	return this->GetCurrentCommandListIndex();
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

Material* IRenderer::GetFullscreenRenderingMaterial() const {
	return materialSystem->LoadMaterial("Resources/Materials/2D/material_rendertarget.json");
}

void IRenderer::RegisterRenderTarget(RenderTarget* renderTarget) {
	if (!resizableRenderTargets.ContainsElement(renderTarget))
		resizableRenderTargets.Insert(renderTarget);
}

void IRenderer::UnregisterRenderTarget(RenderTarget* renderTarget) {
	resizableRenderTargets.Remove(renderTarget);
}

void IRenderer::RegisterPostProcessingPass(IPostProcessPass* pass) {
	if (!resizablePostProcessingPasses.ContainsElement(pass))
		resizablePostProcessingPasses.Insert(pass);
}

void IRenderer::UnregisterPostProcessingPass(IPostProcessPass* pass) {
	resizablePostProcessingPasses.Remove(pass);
}

bool IRenderer::IsRtActive() const {
	return isRtActive;
}

bool IRenderer::IsRtRequested() const {
	return isRtRequested;
}

void IRenderer::AddResizeCallback(std::function<void(const Vector2ui&)> callback) {
	resizeCallbacks.Insert(callback);
}
