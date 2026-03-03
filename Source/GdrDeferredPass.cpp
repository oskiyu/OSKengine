#include "GdrDeferredPass.h"

#include "VertexAttributes.h"

#include "RenderGraph.h"
#include "OSKengine.h"

#include "EntityComponentSystem.h"
#include "GdrGeometryComponent.h"
#include "GdrTexturedComponent.h"
#include "GdrMaterialComponent.h"
#include "TransformComponent3D.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


DynamicArray<VertexAttribBufferRef> OSK::GRAPHICS::VertexAttribBufferRefToList(std::span<const VertexAttributeEntry> entries, const RenderGraph& rg) {
	auto output = DynamicArray<VertexAttribBufferRef>::CreateReserved(entries.size());

	for (const auto& e : entries) {
		output.Insert(VertexAttribBufferRef{
			.bufferRef = {
				.bufferUuid = e.bufferRef,
				.range = rg.GetBuffer(e.bufferRef).GetWholeBufferRange()
				},
			.bindingName = e.name
			});
	}

	return output;
}

GdrDeferredPass::GdrDeferredPass(
	RenderGraph* rg,
	std::span<const GdrBufferUuid> attributes,
	GdrBufferUuid attribsIndexes,
	GdrBufferUuid indexes,
	std::span<const GdrImageUuid> colorTargets,
	GdrImageUuid depthTarget,
	std::span<const GdrBufferUuid> cameraBuffer,
	std::span<const GdrBufferUuid> previousCameraBuffer)
:
	IRenderPass("Resources/Materials/PBR/GpuDriven/gdr_gbuffer.json", RenderPassType::GRAPHICS)
{
	m_indexBuffer = indexes;// Registro de buffer de llamadas de renderizado.
	
	RgBufferRegisterInfo drawCallsInfo_Material{};
	drawCallsInfo_Material.alignment = 0;
	drawCallsInfo_Material.queue = GpuQueueType::MAIN;
	drawCallsInfo_Material.sharedType = GpuSharedMemoryType::GPU_AND_CPU;
	drawCallsInfo_Material.size = sizeof(GdrDrawCall_MaterialInfo) * 1024;
	m_materialsBuffer = rg->RegisterBuffer(drawCallsInfo_Material);

	RgBufferRegisterInfo drawCallsInfo_Model{};
	drawCallsInfo_Model.alignment = 0;
	drawCallsInfo_Model.queue = GpuQueueType::MAIN;
	drawCallsInfo_Model.sharedType = GpuSharedMemoryType::GPU_AND_CPU;
	drawCallsInfo_Model.size = sizeof(GdrDrawCall_Model) * 1024;
	m_instancesBuffer = rg->RegisterBuffer(drawCallsInfo_Model);

	// Setup de la lista de objetos compatible.
	auto signature = ECS::Signature();
	signature.SetTrue(Engine::GetEcs()->GetComponentType<ECS::GdrTexturedComponent>()); 
	signature.SetTrue(Engine::GetEcs()->GetComponentType<ECS::GdrGeometryComponent>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<ECS::GdrMaterialComponent>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<ECS::TransformComponent3D>());
	m_objectListUuid = Engine::GetEcs()->RegisterExternalList(signature);

	OSK_ASSERT_2(
		rg,
		InvalidArgumentException("RenderGraph es null"),
		Engine::GetLogger());
	OSK_ASSERT_2(
		attributes.size() > 0,
		InvalidArgumentException("attributes está vacío"),
		Engine::GetLogger());
	OSK_ASSERT_2(
		colorTargets.size() > 0,
		InvalidArgumentException("colorTargets está vacío"),
		Engine::GetLogger());
	OSK_ASSERT_2(
		!depthTarget.IsEmpty(),
		InvalidArgumentException("depthTarget es null"),
		Engine::GetLogger());

	m_renderGraph = rg;

	// Dependencias parciales.

	// Atributos como tal
	for (const auto& entry : attributes) {
		AddPartialBufferDependency({
			.bufferRefId = entry,
			.usage = RgBufferUsage::STORAGE_BUFFER
			});
	}


	// Índices de los atributos
	AddPartialBufferDependency({
		.bufferRefId = attribsIndexes,
		.usage = RgBufferUsage::STORAGE_BUFFER
		});

	// Índices
	AddPartialBufferDependency({
		.bufferRefId = indexes,
		.usage = RgBufferUsage::INDEX_BUFFER
		});

	// Buffer de instancias:
	AddPartialBufferDependency({
		.bufferRefId = m_instancesBuffer,
		.usage = RgBufferUsage::STORAGE_BUFFER
		});
	AddPartialBufferDependency({
		.bufferRefId = m_materialsBuffer,
		.usage = RgBufferUsage::STORAGE_BUFFER
		});

	// Buffers de cámaras:
	for (const auto& uuid : cameraBuffer) {
		AddPartialBufferDependency({
			.bufferRefId = uuid,
			.usage = RgBufferUsage::UNIFORM_BUFFER
			});
	}
	for (const auto& uuid : previousCameraBuffer) {
		AddPartialBufferDependency({
			.bufferRefId = uuid,
			.usage = RgBufferUsage::UNIFORM_BUFFER
			});
	}

	// Targets
	for (const auto uuid : colorTargets) {
		RgImageDependency dependency{};
		dependency.imageRefId = uuid;

		dependency.range.channel = SampledChannel::COLOR;
		dependency.range.baseLayer = 0;
		dependency.range.baseMipLevel = 0;
		dependency.range.numLayers = 1;
		dependency.range.numMipLevels = 1;

		dependency.arrayIndex = 0;
		dependency.stage = GpuCommandStage::COLOR_ATTACHMENT_OUTPUT;
		dependency.type = RgDependencyType::WRITE;
		dependency.usage = GpuExclusiveImageUsage::COLOR;

		dependency.viewConfig = GpuImageViewConfig::CreateTarget_Color();
		dependency.samplerDesc = GpuImageSamplerDesc::CreateDefault_NoMipMap();

		dependency.arrayIndex = 0;

		AddImageDependency(dependency);
	}

	// Target de profundidad.
	RgImageDependency dependency{};
	dependency.imageRefId = depthTarget;

	dependency.range.channel = SampledChannel::DEPTH;
	dependency.range.baseLayer = 0;
	dependency.range.baseMipLevel = 0;
	dependency.range.numLayers = 1;
	dependency.range.numMipLevels = 1;

	dependency.arrayIndex = 0;
	dependency.stage = GpuCommandStage::DEPTH_STENCIL_START;
	dependency.type = RgDependencyType::WRITE;
	dependency.usage = GpuExclusiveImageUsage::DEPTH;

	dependency.viewConfig = GpuImageViewConfig::CreateTarget_Color();
	dependency.samplerDesc = GpuImageSamplerDesc::CreateDefault_NoMipMap();

	dependency.arrayIndex = 0;

	AddImageDependency(dependency);
}

void GdrDeferredPass::RegisterFullDependencies(
	std::span<const VertexAttribBufferRef> attributes,
	const RgBufferRef& indexes,
	const RgBufferRef& attribsIndexes,
	std::span<const RgBufferRef> cameraBuffer,
	std::span<const RgBufferRef> previousCameraBuffer)
{
	// Atributos como tal
	for (const auto& entry : attributes) {
		RgBufferDependency bufferDependency{};
		bufferDependency.bufferRefId = entry.bufferRef.bufferUuid;
		bufferDependency.range = entry.bufferRef.range;
		bufferDependency.stage = GpuCommandStage::VERTEX_SHADER;
		bufferDependency.type = RgDependencyType::READ;
		bufferDependency.usage = RgBufferUsage::STORAGE_BUFFER;
		bufferDependency.shaderBinding.slotName = "tables";
		bufferDependency.shaderBinding.bindingName = entry.bindingName;

		AddBufferDependency(bufferDependency);
	}

	// Índices de los atributos
	{
		RgBufferDependency bufferDependency{};
		bufferDependency.bufferRefId = attribsIndexes.bufferUuid;
		bufferDependency.range = attribsIndexes.range;
		bufferDependency.stage = GpuCommandStage::VERTEX_SHADER;
		bufferDependency.type = RgDependencyType::READ;
		bufferDependency.usage = RgBufferUsage::STORAGE_BUFFER;
		bufferDependency.shaderBinding.slotName = "tables";
		bufferDependency.shaderBinding.bindingName = "attribsIndices";

		AddBufferDependency(bufferDependency);
	}

	// Índices
	{
		RgBufferDependency bufferDependency{};
		bufferDependency.bufferRefId = indexes.bufferUuid;
		bufferDependency.range = indexes.range;
		bufferDependency.stage = GpuCommandStage::VERTEX_SHADER;
		bufferDependency.type = RgDependencyType::READ;
		bufferDependency.usage = RgBufferUsage::INDEX_BUFFER;

		AddBufferDependency(bufferDependency);
	}

	// Buffer de instancias:
	{
		RgBufferRef drawCallsBuffer_Model{};
		drawCallsBuffer_Model.bufferUuid = m_instancesBuffer;
		drawCallsBuffer_Model.range = m_renderGraph->GetBuffer(m_instancesBuffer).GetWholeBufferRange();

		RgBufferDependency bufferDependency{};
		bufferDependency.bufferRefId = drawCallsBuffer_Model.bufferUuid;
		bufferDependency.range = drawCallsBuffer_Model.range;
		bufferDependency.stage = GpuCommandStage::VERTEX_SHADER;
		bufferDependency.type = RgDependencyType::READ;
		bufferDependency.usage = RgBufferUsage::STORAGE_BUFFER;
		bufferDependency.shaderBinding.slotName = "instances";
		bufferDependency.shaderBinding.bindingName = "models";
		AddBufferDependency(bufferDependency);

		bufferDependency.stage = GpuCommandStage::FRAGMENT_SHADER;
		AddBufferDependency(bufferDependency);
	}
	{
		RgBufferRef drawCallsBuffer_Material{};
		drawCallsBuffer_Material.bufferUuid = m_materialsBuffer;
		drawCallsBuffer_Material.range = m_renderGraph->GetBuffer(m_materialsBuffer).GetWholeBufferRange();

		RgBufferDependency bufferDependency{};
		bufferDependency.bufferRefId = drawCallsBuffer_Material.bufferUuid;
		bufferDependency.range = drawCallsBuffer_Material.range;
		bufferDependency.stage = GpuCommandStage::VERTEX_SHADER;
		bufferDependency.type = RgDependencyType::READ;
		bufferDependency.usage = RgBufferUsage::STORAGE_BUFFER;
		bufferDependency.shaderBinding.slotName = "instances";
		bufferDependency.shaderBinding.bindingName = "materialInfos";
		AddBufferDependency(bufferDependency);

		bufferDependency.stage = GpuCommandStage::FRAGMENT_SHADER;
		AddBufferDependency(bufferDependency);
	}

	// Buffers de cámaras:
	for (USize16 i = 0; i < cameraBuffer.size(); i++) {
		RgBufferDependency bufferDependency{};
		bufferDependency.bufferRefId = cameraBuffer[i].bufferUuid;
		bufferDependency.range = cameraBuffer[i].range;
		bufferDependency.stage = GpuCommandStage::VERTEX_SHADER;
		bufferDependency.type = RgDependencyType::READ;
		bufferDependency.usage = RgBufferUsage::UNIFORM_BUFFER;
		bufferDependency.shaderBinding.slotName = "global";
		bufferDependency.shaderBinding.bindingName = "camera";
		bufferDependency.frameInFlight = i;
		AddBufferDependency(bufferDependency);

		bufferDependency.stage = GpuCommandStage::FRAGMENT_SHADER;
		AddBufferDependency(bufferDependency);
	}

	for (USize16 i = 0; i < previousCameraBuffer.size(); i++) {
		RgBufferDependency bufferDependency{};
		bufferDependency.bufferRefId = previousCameraBuffer[i].bufferUuid;
		bufferDependency.range = previousCameraBuffer[i].range;
		bufferDependency.stage = GpuCommandStage::VERTEX_SHADER;
		bufferDependency.type = RgDependencyType::READ;
		bufferDependency.usage = RgBufferUsage::UNIFORM_BUFFER;
		bufferDependency.shaderBinding.slotName = "global";
		bufferDependency.shaderBinding.bindingName = "previousCamera";
		bufferDependency.frameInFlight = i;
		AddBufferDependency(bufferDependency);

		bufferDependency.stage = GpuCommandStage::FRAGMENT_SHADER;
		AddBufferDependency(bufferDependency);
	}
}

void GdrDeferredPass::Execute(ICommandList* cmdList) {
	const auto& compatibleObjects = Engine::GetEcs()->GetCompatibleObjects(m_objectListUuid);
	auto& instancesBuffer = m_renderGraph->GetBuffer(m_instancesBuffer);
	auto& materialsBuffer = m_renderGraph->GetBuffer(m_materialsBuffer);

	// Reseteamos los buffers.
	instancesBuffer.ResetCursor();
	materialsBuffer.ResetCursor();

	m_drawCalls_Materials.Empty();
	m_drawCalls_Models.Empty();
	m_drawCallsIndexes.Empty();

	instancesBuffer.MapMemory();
	materialsBuffer.MapMemory();


	// Nos aseguramos de tener suficiente espacio
	// para todas las llamadas.
	if (m_drawCalls_Materials.GetReservedSize() < compatibleObjects.size()) {
		m_drawCalls_Materials.Reserve(compatibleObjects.size());
		m_drawCalls_Models.Reserve(compatibleObjects.size());
		m_drawCallsIndexes.Reserve(compatibleObjects.size());
	}


	// El index buffer ya está bound.
	for (const auto& obj : compatibleObjects) {
		const auto& images    = Engine::GetEcs()->GetComponent<ECS::GdrTexturedComponent>(obj);
		const auto& geometry  = Engine::GetEcs()->GetComponent<ECS::GdrGeometryComponent>(obj);
		const auto& material  = Engine::GetEcs()->GetComponent<ECS::GdrMaterialComponent>(obj);
		const auto& transform = Engine::GetEcs()->GetComponent<ECS::TransformComponent3D>(obj); 

		const auto& indexes   = geometry.gdrGeometry->entry.indexesEntry;

		GdrDrawCall_Model model{};
		model.matrix = transform.GetTransform().GetAsMatrix();

		GdrDrawCall_MaterialInfo drawCall{};
		drawCall.albedoImageIdx = (bool)images.colorImageUuid  ? m_imageIndexes[images.colorImageUuid]  : 0;
		drawCall.normalImageIdx = (bool)images.normalImageUuid ? m_imageIndexes[images.normalImageUuid] : 0;
		drawCall.metallicRoughness.x = material.metallic;
		drawCall.metallicRoughness.y = material.roughness;
		drawCall.emissive = material.emmisive;

		instancesBuffer.Write(model);
		materialsBuffer.Write(drawCall);

		m_drawCalls_Materials.Insert(drawCall);
		m_drawCalls_Models   .Insert(model);
		m_drawCallsIndexes   .Insert(indexes);
	}

	instancesBuffer.Unmap();
	materialsBuffer.Unmap();

	for (const auto& drawCall : m_drawCallsIndexes) {
		cmdList->DrawSingleMesh(drawCall.firstIndex, drawCall.indexCount);
	}
}
