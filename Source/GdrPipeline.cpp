#include "GdrPipeline.h"

#include "RenderGraph.h"
#include "GdrDeferredPass.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GdrPipeline::GdrPipeline(RenderGraph* renderGraph) : m_renderGraph(renderGraph) {

}

void GdrPipeline::UpdateCameraBuffers(
	UIndex16 frameIndex,
	const GdrDeferredPass::CameraInfo& cameraInfo,
	const GdrDeferredPass::PreviousCameraInfo& previousCameraInfo)
{
	auto& cameraBuffer = m_renderGraph->GetBuffer(m_cameraBuffer[frameIndex]);
	cameraBuffer.MapMemory();
	cameraBuffer.Write(cameraInfo);

	auto& previousBuffer = m_renderGraph->GetBuffer(m_previousCameraBuffer[frameIndex]);
	previousBuffer.MapMemory();
	previousBuffer.Write(previousCameraInfo);
}

void GdrPipeline::RegisterCameras3D() {
	for (USize64 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		RgBufferRegisterInfo cameraInfo{};
		cameraInfo.alignment = 0;
		cameraInfo.queue = GpuQueueType::MAIN;
		cameraInfo.sharedType = GpuSharedMemoryType::GPU_AND_CPU;
		cameraInfo.size = sizeof(GdrDeferredPass::CameraInfo);
		m_cameraBuffer[i] = m_renderGraph->RegisterBuffer(cameraInfo);

		RgBufferRegisterInfo previousCameraInfo{};
		previousCameraInfo.alignment = 0;
		previousCameraInfo.queue = GpuQueueType::MAIN;
		previousCameraInfo.sharedType = GpuSharedMemoryType::GPU_AND_CPU;
		previousCameraInfo.size = sizeof(GdrDeferredPass::PreviousCameraInfo);
		m_previousCameraBuffer[i] = m_renderGraph->RegisterBuffer(previousCameraInfo);
	}
}

void GdrPipeline::RegisterGBuffer() {
	const auto sizeInfo = RgRelativeSizeImageRegisterArgs::From2D(Vector2f::One);

	const auto gBufferFormats = std::array<Format, 5>{
		Format::RGBA8_UNORM,
		Format::RGB10A2_UNORM,
		Format::RG16_SFLOAT,
		Format::RG16_SFLOAT,
		Format::RGBA8_UNORM
	};

	for (UIndex32 i = 0; i < m_gBufferUuids.size(); i++) {
		RgImageRegisterInfo info{};
		info.dimension = GpuImageDimension::d2D;
		info.numLayers = 1;
		info.format = gBufferFormats[i];
		info.name = std::format("GBuffer Color {}", i);

		m_gBufferUuids[i] = m_renderGraph->RegisterImage(
			info,
			sizeInfo);
	}

	RgImageRegisterInfo depthInfo{};
	depthInfo.dimension = GpuImageDimension::d2D;
	depthInfo.numLayers = 1;
	depthInfo.format = Format::D32_SFLOAT;
	depthInfo.name = "GBuffer Depth";
	m_depthImageUuid = m_renderGraph->RegisterImage(
		depthInfo,
		sizeInfo);
}

void GdrPipeline::RegisterAll() {
	RegisterGBuffer();
	RegisterCameras3D();
	RegisterDeferredPass();
}

void GdrPipeline::FinalRegisterAll() {
	FinalRegisterDeferredPass();
}

void GdrPipeline::RegisterDeferredPass() {
	m_deferredPass = m_renderGraph->RegisterRenderpass(
		MakeUnique<GdrDeferredPass>(
			m_renderGraph,
			m_renderGraph->GetRegisteredAttributesBufferUuids(),	// Buffers de atributos de vértices
			m_renderGraph->GetIndexesBuffer().bufferUuid,			// Índices de los atributos
			m_renderGraph->GetAttribsIndexes().bufferUuid,			// Índices de los vértices
			m_gBufferUuids,											// GBuffer
			m_depthImageUuid,										// Profundidad
			m_cameraBuffer,											// Cámaras
			m_previousCameraBuffer),								// Cámaras del frame anterior
		RenderPassDependencies::Empty());
}

void GdrPipeline::FinalRegisterDeferredPass() {
	auto* deferredPass = m_renderGraph->GetRenderPass(m_deferredPass)->As<GdrDeferredPass>();

	std::array<RgBufferRef, MAX_RESOURCES_IN_FLIGHT> cameraBufferRefs;
	std::array<RgBufferRef, MAX_RESOURCES_IN_FLIGHT> previousCameraBufferRefs;
	for (UIndex16 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		cameraBufferRefs[i].bufferUuid = m_cameraBuffer[i];
		cameraBufferRefs[i].range = m_renderGraph->GetBuffer(m_cameraBuffer[i]).GetWholeBufferRange();

		previousCameraBufferRefs[i].bufferUuid = m_previousCameraBuffer[i];
		previousCameraBufferRefs[i].range = m_renderGraph->GetBuffer(m_previousCameraBuffer[i]).GetWholeBufferRange();
	}
	
	const auto attribsRefs = VertexAttribBufferRefToList(
		m_renderGraph->GetRegisteredAttributes(),
		*m_renderGraph);

	deferredPass->RegisterFullDependencies(
		attribsRefs.GetFullSpan(),
		m_renderGraph->GetIndexesBuffer(),
		m_renderGraph->GetAttribsIndexes(),
		cameraBufferRefs,
		previousCameraBufferRefs);
}
