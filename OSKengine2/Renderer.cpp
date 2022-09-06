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
#include "IPostProcessPass.h"

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

IRenderer::IRenderer(RenderApiType type, bool requestRayTracing) : renderApiType(type), isRtRequested(requestRayTracing) {
	materialSystem = new MaterialSystem;
}

ICommandList* IRenderer::GetPreComputeCommandList() const{
	return preComputeCommandList.GetPointer();
}

ICommandList* IRenderer::GetGraphicsCommandList() const {
	return graphicsCommandList.GetPointer();
}

ICommandList* IRenderer::GetPostComputeCommandList() const{
	return postComputeCommandList.GetPointer();
}

ICommandList* IRenderer::GetFrameBuildCommandList() const {
	return frameBuildCommandList.GetPointer();
}

IGpuMemoryAllocator* IRenderer::GetMemoryAllocator() const {
	return gpuMemoryAllocator.GetPointer();
}

IGpu* IRenderer::GetGpu() const {
	return currentGpu.GetPointer();
}

void IRenderer::UploadLayeredImageToGpu(GpuImage* destination, const TByte* data, TSize numBytes, TSize numLayers, ICommandList* cmdList) {
	TSize gpuNumBytes = destination->GetPhysicalNumberOfBytes() * numLayers;

	const TByte* uploadableData = this->FormatImageDataForGpu(destination, data, numLayers);

	auto stagingBuffer = GetMemoryAllocator()->CreateStagingBuffer(gpuNumBytes);
	stagingBuffer->MapMemory();
	stagingBuffer->Write(uploadableData, gpuNumBytes);
	stagingBuffer->Unmap();

	TSize offsetPerIteration = gpuNumBytes / numLayers;
	for (TSize i = 0; i < numLayers; i++)
		cmdList->CopyBufferToImage(stagingBuffer.GetPointer(), destination, i, offsetPerIteration * i);

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
