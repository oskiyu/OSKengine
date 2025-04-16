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
#include "UnreachableException.h"

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

IRenderer::IRenderer(RenderApiType type, bool requestRayTracing) noexcept
	: m_materialSystem(new MaterialSystem), m_renderApiType(type), m_isRtRequested(requestRayTracing) {
	}


void IRenderer::CloseSingletonInstances() {
	m_swapchain.Delete();
	m_finalRenderTarget.Delete();
	m_materialSystem.Delete();
	m_gpuMemoryAllocator.Delete();
	m_samplers.clear();
}

void IRenderer::CloseGpu() {
	std::lock_guard lock(m_queueSubmitMutex.mutex);

	for (auto& list : m_singleTimeCommandLists) {
		list.Delete();
	}

	m_singleTimeCommandLists.Free();

	if (m_unifiedCommandPools.has_value()) {
		m_unifiedCommandPools.reset();
	}

	m_unifiedQueue.Delete();

	if (m_graphicsComputeCommandPools.has_value()) {
		m_graphicsComputeCommandPools.reset();
	}

	m_graphicsComputeQueue.Delete();

	if (m_transferOnlyCommandPools.has_value()) {
		m_transferOnlyCommandPools.reset();
	}

	m_transferOnlyQueue.Delete();

	m_currentGpu.Delete();
}

ShaderPassFactory* IRenderer::GetShaderPassFactory() {
	return &m_shaderPassFactory;
}

IGpuMemoryAllocator* IRenderer::GetAllocator() {
	return m_gpuMemoryAllocator.GetPointer();
}

IGpu* IRenderer::GetGpu() {
	return m_currentGpu.GetPointer();
}

const IGpu* IRenderer::GetGpu() const {
	return m_currentGpu.GetPointer();
}

void IRenderer::UploadLayeredImageToGpu(GpuImage* destination, const TByte* data, USize64 numBytes, USize32 numLayers, ICommandList* cmdList) {
	OSK_ASSERT(numLayers > 0, InvalidArgumentException("El número de capas debe ser > 0."));
	OSK_ASSERT(numBytes > 0, InvalidArgumentException("El número de bytes debe ser > 0."));

	USize64 gpuNumBytes = destination->GetPhysicalNumberOfBytes() * numLayers;

	const TByte* uploadableData = data;

	auto stagingBuffer = GetAllocator()->CreateStagingBuffer(gpuNumBytes, cmdList->GetCompatibleQueueType());
	stagingBuffer->MapMemory();
	stagingBuffer->Write(uploadableData, gpuNumBytes);
	stagingBuffer->Unmap();

	USize64 offsetPerIteration = gpuNumBytes / numLayers;
	for (UIndex32 i = 0; i < numLayers; i++)
		cmdList->CopyBufferToImage(stagingBuffer.GetValue(), destination, i, offsetPerIteration * i);

	cmdList->RegisterStagingBuffer(std::move(stagingBuffer));
}

void IRenderer::SetPresentMode(PresentMode mode) {
	m_swapchain->SetPresentMode(m_currentGpu.GetValue(), mode);
}

void IRenderer::HandleResize(const Vector2ui& resolution) {
	if (Engine::GetEcs()) {
		for (auto i : Engine::GetEcs()->GetRenderSystems()) {
			i->Resize(resolution);
		}
	}

	m_finalRenderTarget->Resize(resolution);

	Engine::GetEcs()->PublishEvent<IDisplay::ResolutionChangedEvent>({ resolution });
}

void IRenderer::UploadImageToGpu(GpuImage* destination, const TByte* data, USize64 numBytes, ICommandList* cmdList) {
	UploadLayeredImageToGpu(destination, data, numBytes, 1, cmdList);
}

void IRenderer::UploadCubemapImageToGpu(GpuImage* destination, const TByte* data, USize64 numBytes, ICommandList* cmdList) {
	UploadLayeredImageToGpu(destination, data, numBytes, 6, cmdList);
}

UniquePtr<ICommandList> IRenderer::CreateSingleUseCommandList(GpuQueueType queueType, std::thread::id threadId) {
	ICommandPool* creator = nullptr;

	switch (queueType) {
	case OSK::GRAPHICS::GpuQueueType::MAIN:
		creator = UseUnifiedCommandPool()
			? GetUnifiedCommandPool(threadId)
			: GetGraphicsComputeCommandPool(threadId);
		break;

	case OSK::GRAPHICS::GpuQueueType::PRESENTATION:
		// No hay lista de comandos de PRESENTATION.
		OSK_ASSERT(false, InvalidArgumentException("No se puede crear una lista de comandos en la cola GpuQueueType::PRESENTATION."));
		break;

	case OSK::GRAPHICS::GpuQueueType::ASYNC_TRANSFER:
		if (HasTransferOnlyCommandPool()) {
			creator = GetTransferOnlyCommandPool(threadId);
		}
		else {
			creator = UseUnifiedCommandPool()
				? GetUnifiedCommandPool(threadId)
				: GetGraphicsComputeCommandPool(threadId);
		}
		break;

	default:
		break;
	}

	return creator->CreateSingleTimeCommandList(*GetGpu());
}

MaterialSystem* IRenderer::GetMaterialSystem() {
	return m_materialSystem.GetPointer();
}

void IRenderer::CreateMainRenderpass() {
	RenderTargetAttachmentInfo colorInfo{ .format = m_swapchain->GetColorFormat(), .name = "Final Color Target" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D32S8_SFLOAT_SUINT, .name = "Final Depth Target" };

	m_finalRenderTarget = MakeUnique<RenderTarget>();
	m_finalRenderTarget->CreateAsFinal(m_swapchain->GetImage(0)->GetSize2D(),
		{ colorInfo }, depthInfo);
}

RenderApiType IRenderer::GetRenderApi() const {
	return m_renderApiType;
}

USize32 IRenderer::GetSwapchainImagesCount() const {
	return m_swapchain->GetImageCount();
}

RenderTarget* IRenderer::GetFinalRenderTarget() {
	return m_finalRenderTarget.GetPointer();
}

UIndex32 IRenderer::GetCurrentResourceIndex() const {
	return this->GetCurrentCommandListIndex();
}

ISwapchain* IRenderer::_GetSwapchain() {
	return m_swapchain.GetPointer();
}

bool IRenderer::_HasImplicitResizeHandling() const {
	return m_implicitResizeHandling;
}

ICommandList* IRenderer::GetMainCommandList() {
	return m_mainCommandList.GetPointer();
}

bool IRenderer::IsOpen() const {
	return m_currentGpu.HasValue();
}

bool IRenderer::IsRtActive() const {
	return m_isRtActive;
}

bool IRenderer::IsRtRequested() const {
	return m_isRtRequested;
}

const IGpuImageSampler& IRenderer::GetSampler(const GpuImageSamplerDesc& info) const {
	// Si ya existe:
	{
		// Cerramos en modo read.
		std::shared_lock lock(m_samplerMapMutex);

		auto samplerIt = m_samplers.find(info);

		if (samplerIt != m_samplers.end()) {
			return samplerIt->second.GetValue();
		}
	}

	// Si no, añadimos.
	//
	// Cerramos en modo escritura.
	std::unique_lock lock(m_samplerMapMutex);

	UniquePtr<IGpuImageSampler> sampler = m_currentGpu->CreateSampler(info);
	auto& output = sampler.GetValue();

	m_samplers[info] = std::move(sampler);

	return output;
}

void IRenderer::RegisterUnifiedCommandPool(const ICommandQueue* unifiedQueue) {
	m_unifiedCommandPools.emplace(unifiedQueue);
}

void IRenderer::RegisterGraphicsCommputeCommandPool(const ICommandQueue* graphicsComputeQueue) {
	m_graphicsComputeCommandPools.emplace(graphicsComputeQueue);
}

void IRenderer::RegisterTransferOnlyCommandPool(const ICommandQueue* transferQueue) {
	m_transferOnlyCommandPools.emplace(transferQueue);
}


void IRenderer::_SetUnifiedCommandQueue(UniquePtr<ICommandQueue>&& commandQueue) {
	m_unifiedQueue = std::move(commandQueue);
}

void IRenderer::_SetGraphicsCommputeCommandQueue(UniquePtr<ICommandQueue>&& commandQueue) {
	m_graphicsComputeQueue = std::move(commandQueue);
}

void IRenderer::_SetPresentationCommandQueue(UniquePtr<ICommandQueue>&& commandQueue) {
	m_presentationQueue = std::move(commandQueue);
}

void IRenderer::_SetTransferOnlyCommandQueue(UniquePtr<ICommandQueue>&& commandQueue) {
	m_transferOnlyQueue = std::move(commandQueue);
}

void IRenderer::_SetMainCommandList(UniquePtr<ICommandList>&& commandList) {
	m_mainCommandList = std::move(commandList);
}

void IRenderer::_SetGpu(UniquePtr<IGpu>&& gpu) {
	m_currentGpu = std::move(gpu);
}

void IRenderer::_SetMemoryAllocator(UniquePtr<IGpuMemoryAllocator>&& allocator) {
	m_gpuMemoryAllocator = std::move(allocator);
}

void IRenderer::_SetSwapchain(UniquePtr<ISwapchain>&& swapchain) {
	m_swapchain = std::move(swapchain);
}


ICommandPool* IRenderer::GetUnifiedCommandPool(std::thread::id threadId) {
	return m_unifiedCommandPools->GetCommandPool(threadId);
}

bool IRenderer::UseUnifiedCommandPool() const {
	return m_unifiedCommandPools.has_value();
}

const ICommandQueue* IRenderer::GetUnifiedQueue() const {
	return m_unifiedQueue.GetPointer();
}

ICommandQueue* IRenderer::GetUnifiedQueue() {
	return m_unifiedQueue.GetPointer();
}

const ICommandQueue* IRenderer::GetGraphicsComputeQueue() const {
	return m_graphicsComputeQueue.GetPointer();
}

ICommandQueue* IRenderer::GetGraphicsComputeQueue() {
	return m_graphicsComputeQueue.GetPointer();
}

const ICommandQueue* IRenderer::GetPresentationQueue() const {
	return m_presentationQueue.GetPointer();
}

bool IRenderer::UseUnifiedCommandQueue() const {
	return m_unifiedQueue.HasValue();
}

const ICommandQueue* IRenderer::GetTransferOnlyQueue() const {
	return m_transferOnlyQueue.GetPointer();
}

bool IRenderer::HasTransferOnlyQueue() const {
	return m_transferOnlyQueue.HasValue();
}

const ICommandQueue* IRenderer::GetMainRenderingQueue() const {
	return UseUnifiedCommandQueue()
		? GetUnifiedQueue()
		: GetGraphicsComputeQueue();
}

const ICommandQueue* IRenderer::GetOptimalQueue(GpuQueueType type) const {
	switch (type) {

	case OSK::GRAPHICS::GpuQueueType::MAIN:
		return GetMainRenderingQueue();

		break;

	case OSK::GRAPHICS::GpuQueueType::PRESENTATION:
		if (type == GpuQueueType::PRESENTATION) {
			return UseUnifiedCommandQueue()
				? GetUnifiedQueue()
				: GetPresentationQueue();
		}

		break;

	case OSK::GRAPHICS::GpuQueueType::ASYNC_TRANSFER:

		if (HasTransferOnlyCommandPool()) {
			return GetTransferOnlyQueue();
		}
		else {
			return GetMainRenderingQueue();
		}

		break;

	default:
		throw UnreachableException("GpuQueueType no reconocido.");
		return nullptr;
	}
}

ICommandPool* IRenderer::GetGraphicsComputeCommandPool(std::thread::id threadId) {
	return m_graphicsComputeCommandPools->GetCommandPool(threadId);
}

ICommandPool* IRenderer::GetTransferOnlyCommandPool(std::thread::id threadId) {
	return m_transferOnlyCommandPools->GetCommandPool(threadId);
}

bool IRenderer::HasTransferOnlyCommandPool() const {
	return m_transferOnlyCommandPools.has_value();
}

Material* IRenderer::GetFullscreenRenderingMaterial() {
	return m_materialSystem->LoadMaterial("Resources/Materials/2D/material_rendertarget.json");
}
