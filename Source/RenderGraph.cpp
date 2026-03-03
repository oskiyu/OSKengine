#include "RenderGraph.h"

#include "HashMap.hpp"
#include "RgImageDependency.h"
#include "GpuImageLayout.h"

#include "ICommandList.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"

#include "GpuModel3D.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

constexpr auto MAX_VERTICES = 4'000'000;

GpuImageLayout GetLayout(GpuExclusiveImageUsage usage) {
	switch (usage) {
	case GpuExclusiveImageUsage::UNKNOWN:
		break;

	case GpuExclusiveImageUsage::COLOR:
		return GpuImageLayout::COLOR_ATTACHMENT;

	case GpuExclusiveImageUsage::DEPTH:
		return GpuImageLayout::DEPTH_STENCIL_TARGET;

	case GpuExclusiveImageUsage::SAMPLED:
		return GpuImageLayout::SAMPLED;

	case GpuExclusiveImageUsage::TRANSFER_SOURCE:
		return GpuImageLayout::TRANSFER_SOURCE;

	case GpuExclusiveImageUsage::TRANSFER_DESTINATION:
		return GpuImageLayout::TRANSFER_DESTINATION;

	case GpuExclusiveImageUsage::COMPUTE_OR_RT_TARGET:
		return GpuImageLayout::GENERAL;

	case GpuExclusiveImageUsage::STENCIL:
		return GpuImageLayout::DEPTH_STENCIL_TARGET;

	};
}

GpuAccessStage GetAccessStage(GpuExclusiveImageUsage usage, RgDependencyType type) {
	switch (usage) {
	case GpuExclusiveImageUsage::UNKNOWN:
		break;

	case GpuExclusiveImageUsage::COLOR:
		// TODO: GpuAccessStage::COLOR_ATTACHMENT_READ ?
		return GpuAccessStage::COLOR_ATTACHMENT_WRITE;

	case GpuExclusiveImageUsage::DEPTH:
		return GpuAccessStage::DEPTH_STENCIL_READ | GpuAccessStage::DEPTH_STENCIL_WRITE;

	case GpuExclusiveImageUsage::SAMPLED:
		return GpuAccessStage::SAMPLED_READ;

	case GpuExclusiveImageUsage::TRANSFER_SOURCE:
		return GpuAccessStage::TRANSFER_READ;

	case GpuExclusiveImageUsage::TRANSFER_DESTINATION:
		return GpuAccessStage::TRANSFER_WRITE;

	case GpuExclusiveImageUsage::COMPUTE_OR_RT_TARGET:
		// TODO: seleccionar si IN/OUT.
		return
			(type == RgDependencyType::READ || type == RgDependencyType::READ_WRITE ? GpuAccessStage::STORAGE_IMAGE_READ : GpuAccessStage::NONE) |
			(type == RgDependencyType::WRITE || type == RgDependencyType::READ_WRITE ? GpuAccessStage::SHADER_WRITE : GpuAccessStage::NONE);

	case GpuExclusiveImageUsage::STENCIL:
		return GpuAccessStage::DEPTH_STENCIL_READ | GpuAccessStage::DEPTH_STENCIL_WRITE;

	}
}

GpuAccessStage GetAccessStage(RgBufferUsage usage, RgDependencyType type) {
	switch (usage) {
	case RgBufferUsage::NONE:
		break;

	case RgBufferUsage::UNIFORM_BUFFER:
		return GpuAccessStage::MEMORY_READ;

	case RgBufferUsage::STORAGE_BUFFER:
		return
			(type == RgDependencyType::READ  || type == RgDependencyType::READ_WRITE ? 
				GpuAccessStage::STORAGE_BUFFER_READ : GpuAccessStage::NONE) |

			(type == RgDependencyType::WRITE || type == RgDependencyType::READ_WRITE ? 
				GpuAccessStage::MEMORY_WRITE : GpuAccessStage::NONE);

	}
}

RenderGraph::RenderGraph(IRenderer* renderer) 
	:
	m_bufferManager(MakeUnique<RgBufferManager>(renderer)),
	m_imageManager(MakeUnique<RgImageManager>(renderer)),
	m_geometryManager(renderer, m_bufferManager.GetPointer(), MAX_VERTICES),
	m_renderer(renderer)
{
	m_geometryManager.PartiallyRegisterUnifiedGeometryBuffers();
}

GdrBufferUuid RenderGraph::RegisterBuffer(const RgBufferRegisterInfo& info) {
	return m_bufferManager->RegisterBuffer(info);
}

GdrImageUuid RenderGraph::RegisterImage(
	const RgImageRegisterInfo& info, 
	RgRelativeSizeImageRegisterArgs size) 
{
	return m_imageManager->RegisterImage(info, size);
}

GdrImageUuid RenderGraph::RegisterImage(
	const RgImageRegisterInfo& info, 
	RgAbsoluteSizeImageRegisterArgs size) 
{
	return m_imageManager->RegisterImage(info, size);
}

GdrImageUuid RenderGraph::RegisterExternalImage(GpuImage* image) {
	return m_imageManager->RegisterExternalImage(image);
}

void RenderGraph::RegisterVertexAttribute(
	VertexAttribUuid uuid,
	std::string name,
	USize32 attributeSize)
{
	m_geometryManager.RegisterVertexAttribute(
		uuid, 
		std::move(name), 
		attributeSize,
		m_bufferManager->ReserveUuid());
}

GpuGeometryEntry RenderGraph::RegisterGeometry(const GpuGeometryDesc& geometry) {
	return m_geometryManager.RegisterGeometry(geometry);
}

std::span<const VertexAttributeEntry> RenderGraph::GetRegisteredAttributes() const {
	return m_geometryManager.GetRegisteredAttribs();
}

std::span<const GdrBufferUuid> RenderGraph::GetRegisteredAttributesBufferUuids() const {
	return m_geometryManager.GetRegisteredAttribsBufferUuids();
}

RgBufferRef RenderGraph::GetAttribsIndexes() const {
	return m_geometryManager.GetAttribsIndexes();
}

RgBufferRef RenderGraph::GetIndexesBuffer() const {
	return m_geometryManager.GetIndexesBuffer();
}

void RenderGraph::CreateGeometryBuffers() {
	m_geometryManager.RegisterUnifiedGeometryBuffers();
}

RenderPassUuid RenderGraph::RegisterRenderpass(
	UniquePtr<IRenderPass>&& pass,
	const RenderPassDependencies& passDependencies)
{
	const auto uuid = StaticUuidProvider::New<RenderPassUuid>();

	m_renderPassesMap[uuid] = pass.GetPointer();
	m_renderPassesEntries.Insert( { std::move(pass), passDependencies });

	return uuid;
}

void RenderGraph::Compile() {
	m_geometryManager.RegisterUnifiedGeometryBuffers();

	m_bufferManager->CreateBuffers(m_renderPassesEntries.GetFullSpan());
	m_imageManager ->CreateImages (m_renderPassesEntries.GetFullSpan());
	
	m_geometryManager.PrepareUnifiedGeometryBuffersReferences();

	Flatten();
}

void RenderGraph::Execute(ICommandList* cmdList) {
	for (auto& entry : m_renderPassesEntries) {
		ExecuteNext(cmdList, entry.renderPass.GetPointer());
	}
}

void RenderGraph::ExecuteNext(ICommandList* cmdList, IRenderPass* renderPass) {
	const auto currentFrameIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	// Transición de buffers
	for (const auto& bufferDependency : renderPass->GetBufferDependencies()) {

		// Sólo se procesan las dependencias aplicables a este frame.
		// No es aplicable si el frame in flight indicado no coincide.
		if (bufferDependency.frameInFlight && *bufferDependency.frameInFlight != currentFrameIndex) {
			continue;
		}

		auto& buffer = m_bufferManager->GetBuffer(bufferDependency.bufferRefId);

		cmdList->SetGpuBufferBarrier(
			&buffer,
			bufferDependency.range,
			GpuBarrierInfo(
				bufferDependency.stage,
				GetAccessStage(bufferDependency.usage, bufferDependency.type)
			));
	}

	// Transición de imágenes
	for (const auto& imgDependency : renderPass->GetImageDependencies()) {
		auto& img = m_imageManager->GetImage(imgDependency.imageRefId);

		// Sólo se procesan las dependencias aplicables a este frame.
		// No es aplicable si el frame in flight indicado no coincide.
		if (imgDependency.frameInFlight && *imgDependency.frameInFlight != currentFrameIndex) {
			continue;
		}

		cmdList->SetGpuImageBarrier(
			&img,
			GetLayout(imgDependency.usage),
			GpuBarrierInfo(
				imgDependency.stage,
				GetAccessStage(imgDependency.usage, imgDependency.type)
			),
			imgDependency.range);
	}

	renderPass->_Execute(cmdList, *this);
}

void RenderGraph::Flatten() {
	m_flatRenderPass.Empty();
	m_flatRenderPass.Reserve(m_renderPassesEntries.GetSize());

	struct Entry {
		RenderPassEntry* renderPassEntry;
		bool valid = true;
	};
	std::list<Entry> orderedRenderPasses;

	for (auto& pass : m_renderPassesEntries) {
		orderedRenderPasses.push_back({ &pass });
	}

	bool dirty = true;
	UIndex64 count = 0;
	while (dirty) {
		dirty = false;

		for (auto it = orderedRenderPasses.begin(); it != orderedRenderPasses.end(); ++it) {
			if (!it->valid) {
				continue;
			}

			// Por detras:
			for (auto it2 = orderedRenderPasses.begin(); it2 != it; ++it2) {
				if (!it2->valid) {
					continue;
				}

				auto* segundoRenderPass = it ->renderPassEntry;
				auto* primerRenderPass  = it2->renderPassEntry;

				if (primerRenderPass->dependencies.executeAfterThese.contains(segundoRenderPass->renderPass->GetName()))
				{
					orderedRenderPasses.insert(it2, { primerRenderPass });
					it->valid = false;
					dirty = true;

					break;
				}
			}

			if (dirty) {
				break;
			}

			// Por delante:
			for (auto it2 = it; it2 != orderedRenderPasses.end(); ++it2) {
				if (!it2->valid) {
					continue;
				}

				auto* primerRenderPass  = it ->renderPassEntry;
				auto* segundoRenderPass = it2->renderPassEntry;

				if (primerRenderPass->dependencies.executeAfterThese.contains(segundoRenderPass->renderPass->GetName()))
				{
					it2++;
					orderedRenderPasses.insert(it2, { primerRenderPass });
					it->valid = false;
					dirty = true;

					break;
				}
			}

			count++;
		}
	}

	for (auto& entry : orderedRenderPasses) {
		m_flatRenderPass.Insert(entry.renderPassEntry->renderPass.GetPointer());
	}
}

GpuBuffer& RenderGraph::GetBuffer(GdrBufferUuid uuid) {
	return m_bufferManager->GetBuffer(uuid);
}

const GpuBuffer& RenderGraph::GetBuffer(GdrBufferUuid uuid) const {
	return m_bufferManager->GetBuffer(uuid);
}

GpuImage& RenderGraph::GetImage(GdrImageUuid uuid) {
	return m_imageManager->GetImage(uuid);
}

const GpuImage& RenderGraph::GetImage(GdrImageUuid uuid) const {
	return m_imageManager->GetImage(uuid);
}

void RenderGraph::SetFramebufferResolution(USize32 res) {
	m_imageManager->SetFramebufferResolution(res);
}

void RenderGraph::SetFramebufferResolution(Vector2ui res) {
	m_imageManager->SetFramebufferResolution(res);
}

void RenderGraph::SetFramebufferResolution(Vector3ui res) {
	m_imageManager->SetFramebufferResolution(res);
}

IRenderPass* RenderGraph::GetRenderPass(RenderPassUuid uuid) {
	return m_renderPassesMap.at(uuid);
}

const IRenderPass* RenderGraph::GetRenderPass(RenderPassUuid uuid) const {
	return m_renderPassesMap.at(uuid);
}
