#include "GdrDeferredSystem.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"

#include "ModelComponent3D.h"
#include "TransformComponent3D.h"

#include "Vertex3D.h"
#include "VertexAttributes.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


GdrDeferredRenderSystem::GdrDeferredRenderSystem() : DeferredRenderSystem() {
}

void GdrDeferredRenderSystem::SetMaxCounts(USize32 maxVertexCount, USize32 maxMeshCount) {
	CreateUnifiedBuffers(maxVertexCount, maxMeshCount);
}

void GdrDeferredRenderSystem::CreateUnifiedBuffers(USize32 maxVertexCount, USize32 maxMeshCount) {
	/*
	auto* allocator = Engine::GetRenderer()->GetAllocator();

	for (UIndex64 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		const USize64 uboAlignment = 0; // TODO

		const auto createBufferFunc = [allocator, uboAlignment](USize64 size) {
			return allocator->CreateBuffer(size, uboAlignment, GpuBufferUsage::STORAGE_BUFFER, GpuSharedMemoryType::GPU_AND_CPU, GpuQueueType::MAIN).GetPointer();
		};

		m_unifiedVertexBuffers[i] = allocator->CreateBuffer(sizeof(GdrVertex3D) * maxVertexCount, 0, GpuBufferUsage::VERTEX_BUFFER, GpuSharedMemoryType::GPU_AND_CPU, GpuQueueType::MAIN).GetPointer();
		m_unifiedIndexBuffers[i] = allocator->CreateBuffer(sizeof(TIndexSize) * maxVertexCount * 3, 0, GpuBufferUsage::INDEX_BUFFER, GpuSharedMemoryType::GPU_AND_CPU, GpuQueueType::MAIN).GetPointer();

		// Attributes
		m_vertexPositionsBuffers[i] = createBufferFunc(sizeof(VertexPositionAttribute3D) * maxVertexCount);
		m_vertexAttributesBuffers[i] = createBufferFunc(sizeof(VertexAttributes3D) * maxVertexCount);
		m_vertexAnimAttributesBuffers[i] = createBufferFunc(sizeof(VertexAnimationAttributes3D) * maxVertexCount);

		// Tables
		m_perMaterialTable[i] = createBufferFunc(sizeof(GdrPerMeshInfo) * maxMeshCount);
		m_perMeshTable[i] = createBufferFunc(sizeof(GdrPerMaterialInfo) * maxMeshCount);

		m_perInstanceBuffers[i] = createBufferFunc(sizeof(GdrPerMeshInstanceInfo) * maxMeshCount);
	}

	// Material
	m_gdrMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/GpuDriven/gdr_gbuffer.json");
	m_gdrMaterialInstance = m_gdrMaterial->CreateInstance().GetPointer();


	std::array<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> cameraUbos{};
	std::array<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> previousCameraUbos{};

	std::array<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> meshTable{};
	std::array<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> positionsTable{};
	std::array<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> attributesTable{};

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		cameraUbos[i] = m_cameraBuffers[i].GetPointer();
		previousCameraUbos[i] = m_previousFrameCameraBuffers[i].GetPointer();

		meshTable[i] = m_perMeshTable[i].GetPointer();
		positionsTable[i] = m_vertexPositionsBuffers[i].GetPointer();
		attributesTable[i] = m_vertexAttributesBuffers[i].GetPointer();
	}

	m_gdrMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", cameraUbos);
	m_gdrMaterialInstance->GetSlot("global")->SetUniformBuffers("previousCamera", previousCameraUbos);
	m_gdrMaterialInstance->GetSlot("global")->FlushUpdate();

	m_gdrMaterialInstance->GetSlot("tables")->SetStorageBuffers("meshInfos", meshTable);
	m_gdrMaterialInstance->GetSlot("tables")->SetStorageBuffers("positions", positionsTable);
	m_gdrMaterialInstance->GetSlot("tables")->SetStorageBuffers("attributes", attributesTable);
	m_gdrMaterialInstance->GetSlot("tables")->FlushUpdate();
	*/
}

void GdrDeferredRenderSystem::Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) {
	const auto resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	m_unifiedVertexBuffers[resourceIndex]->ResetCursor();
	m_unifiedIndexBuffers[resourceIndex]->ResetCursor();

	m_vertexPositionsBuffers[resourceIndex]->ResetCursor();
	m_vertexAttributesBuffers[resourceIndex]->ResetCursor();
	m_vertexAnimAttributesBuffers[resourceIndex]->ResetCursor();

	m_perMeshTable[resourceIndex]->ResetCursor();
	m_perMaterialTable[resourceIndex]->ResetCursor();
	m_perInstanceBuffers[resourceIndex]->ResetCursor();


	m_unifiedVertexBuffers[resourceIndex]->MapMemory();
	m_unifiedIndexBuffers[resourceIndex]->MapMemory();

	m_vertexPositionsBuffers[resourceIndex]->MapMemory();
	m_vertexAttributesBuffers[resourceIndex]->MapMemory();
	m_vertexAnimAttributesBuffers[resourceIndex]->MapMemory();

	m_perMeshTable[resourceIndex]->MapMemory();
	m_perMaterialTable[resourceIndex]->MapMemory();
	m_perInstanceBuffers[resourceIndex]->MapMemory();


	m_draws.clear();

	UIndex32 nextMeshIndex = 0;
	TIndexSize nextVertexIndex = 0;

	USize32 numVertices = 0;
	USize32 numIndices = 0;

	GdrPerMeshInfo currentOffsets{};

	UIndex32 nextImageIndex = 2;

	

	const auto* defaultNormalTexture = OSK::Engine::GetRenderer()->GetAllocator()->GetDefaultNormalTexture();
	m_gdrMaterialInstance->GetSlot("images")->SetGpuImage(
		"images", 
		*defaultNormalTexture->GetView(GpuImageViewConfig::CreateSampled_Default()), 
		GpuImageSamplerDesc::CreateDefault_WithMipMap(*defaultNormalTexture),
		0);
		


	const auto jitterIndex = m_taaProvider.GetCurrentFrameJitterIndex();
	const Vector2ui resolution = m_gBuffer.GetImage(GBuffer::Target::COLOR)->GetSize2D();

	// TODO: passes
	for (const GameObjectIndex obj : objects) {
		const ModelComponent3D& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const auto& transform = Engine::GetEcs()->GetComponent<TransformComponent3D>(obj).GetTransform();

		
		for (UIndex32 i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			const auto& mesh = model.GetModel()->GetMeshes()[i];
			// const bool isInsideFrustum = mesh.GetBounds().IsInsideFrustum(frustum);

			const USize32 nNumVertices = model.GetModel()->GetVertexBuffer().GetVertexView().numVertices;
			const USize32 nNumIndices = model.GetModel()->GetIndexBuffer().GetIndexView().numIndices;

			// Si el mesh no ha sido registrado...
			if (m_meshes.contains(mesh.GetUuid())) {

				WriteMeshInfo(currentOffsets);

				WriteMeshUnifiedVertexAndIndexBuffers(mesh, *model.GetModel(), nextVertexIndex, nextMeshIndex);
				nextVertexIndex += model.GetModel()->GetVertexBuffer().GetIndexView().numIndices;
				nextMeshIndex++;

				WriteMeshVertexAttributes(model.GetModel()->GetVertexAttributesMap(), &currentOffsets);

				WriteMaterialInfo(*model.GetModel(), i, &currentOffsets, &nextImageIndex);
			}

			GdrPerMeshInstanceInfo drawInfo{};
			drawInfo.modelMatrix = transform.GetAsMatrix();
			drawInfo.previousModelMatrix = m_previousFrameMatrices.contains(obj)
				? m_previousFrameMatrices.at(obj)
				: glm::mat4(1.0f);

			m_draws[mesh.GetUuid()].Insert(drawInfo);

		}

		m_previousFrameMatrices[obj] = transform.GetAsMatrix();
	}

	for (const auto& [mesh, draws] : m_draws) {
		for (const auto& draw : draws) {
			m_perInstanceBuffers[resourceIndex]->Write(draw);
		}
	}

	m_gdrMaterialInstance->GetSlot("global")->FlushUpdate();
	m_gdrMaterialInstance->GetSlot("tables")->FlushUpdate();
	m_gdrMaterialInstance->GetSlot("images")->FlushUpdate();

	m_unifiedVertexBuffers[resourceIndex]->Unmap();
	m_unifiedIndexBuffers[resourceIndex]->Unmap();

	m_vertexPositionsBuffers[resourceIndex]->Unmap();
	m_vertexAttributesBuffers[resourceIndex]->Unmap();
	m_vertexAnimAttributesBuffers[resourceIndex]->Unmap();

	m_perMeshTable[resourceIndex]->Unmap();
	m_perMaterialTable[resourceIndex]->Unmap();
	m_perInstanceBuffers[resourceIndex]->Unmap();


	// Rendering
	commandList->StartDebugSection("PBR-Bindless GBuffer", Color::Red);
	m_gBuffer.BeginRenderpass(commandList, Color::Black * 0.0f, false);

	SetupViewport(commandList);

	commandList->BindMaterial(*m_gdrMaterial);
	commandList->BindMaterialInstance(m_gdrMaterialInstance.GetValue());

	struct {
		Vector3f info{};
	} pushConstants{
		.info = Vector3f(
			static_cast<float>(resolution.x),
			static_cast<float>(resolution.y),
			static_cast<float>(jitterIndex))
	};

	commandList->PushMaterialConstants("pushConstants", pushConstants);

	UIndex32 perInstanceIndex = 0;
	for (const auto& [mesh, draws] : m_draws) {
		VertexBufferView vertexView{};
		vertexView.vertexInfo = GdrVertex3D::GetVertexInfo();
		vertexView.numVertices = numVertices;
		vertexView.offsetInBytes = 0;
		commandList->BindVertexBufferRange(m_unifiedVertexBuffers[resourceIndex].GetValue(), vertexView);

		IndexBufferView indexView{};
		indexView.type = IndexType::U32;
		indexView.offsetInBytes = 0;
		indexView.numIndices = numIndices;
		commandList->BindIndexBufferRange(m_unifiedIndexBuffers[resourceIndex].GetValue(), indexView);

		commandList->DrawInstances(0, numIndices, perInstanceIndex, static_cast<USize32>(draws.GetSize()));

		perInstanceIndex += static_cast<USize32>(draws.GetSize());
	}

	commandList->EndGraphicsRenderpass();
	commandList->EndDebugSection();
}

void GdrDeferredRenderSystem::WriteMeshUnifiedVertexAndIndexBuffers(const GpuMesh3D& mesh, const GpuModel3D& model, TIndexSize firstIndex, UIndex32 nextGdrIndex) {
	const auto resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	
	const USize32 numVertices = model.GetVertexBuffer().GetVertexView().numVertices;
	const USize32 numIndices = model.GetIndexBuffer().GetIndexView().numIndices;

	// Escribimos los buffers unificados.
	for (UIndex32 v = 0; v < numVertices; v++) {
		m_unifiedVertexBuffers[resourceIndex]->Write(GdrVertex3D{ .gdrIndex = nextGdrIndex });
	}

	for (UIndex32 idx = 0; idx < numIndices; idx++) {
		m_unifiedIndexBuffers[resourceIndex]->Write(firstIndex + idx);
	}

	// Establecemos las posiciones de los meshes.
	m_meshes[mesh.GetUuid()].firstVertexIndex = firstIndex;
	m_meshes[mesh.GetUuid()].numIndices = numIndices;
}

void GdrDeferredRenderSystem::WriteMeshVertexAttributes(const VerticesAttributesMaps& attributesMap, GdrPerMeshInfo* previousOffsets) {
	const auto resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const auto& positions = attributesMap.GetVerticesAttributes<VertexPositionAttribute3D>();
	const auto& attributes = attributesMap.GetVerticesAttributes<VertexAttributes3D>();

	previousOffsets->positionsOffset += static_cast<USize32>(positions.GetSize());
	previousOffsets->attributesOffset += static_cast<USize32>(attributes.GetSize());

	for (UIndex64 a = 0; a < positions.GetSize(); a++) {
		m_vertexPositionsBuffers[resourceIndex]->Write(positions[a]);
		m_vertexAttributesBuffers[resourceIndex]->Write(attributes[a]);
	}

	if (attributesMap.HasAttribute(VertexAnimationAttributes3D::GetAttribName())) {
		const auto& animAttribs = attributesMap.GetVerticesAttributes<VertexAttributes3D>();

		previousOffsets->animationAttributesOffset += static_cast<USize32>(animAttribs.GetSize());

		for (UIndex64 a = 0; a < animAttribs.GetSize(); a++) {
			m_vertexAnimAttributesBuffers[resourceIndex]->Write(animAttribs[a]);
		}
	}
}

void GdrDeferredRenderSystem::WriteMeshInfo(const GdrPerMeshInfo& info) {
	const auto resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	m_perMeshTable[resourceIndex]->Write(info);
}

void GdrDeferredRenderSystem::WriteMaterialInfo(const GpuModel3D& model, UIndex32 meshIndexInsideModel, GdrPerMeshInfo* previousOffsets, UIndex32* nextImageIndex) {
	const UIndex32 defaultColorIndex = 0;
	const UIndex32 defaultNormalIndex = 1;
	
	const auto resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_Default();

	const auto& mesh = model.GetMeshes()[meshIndexInsideModel];

	const auto& material = model.GetMaterial(mesh.GetMaterialIndex());

	const bool containsColorTexture = material.colorTextureIndex.has_value();
	const bool containsNormalTexture = material.normalTextureIndex.has_value();

	const UIndex32 colorViewIndex = containsColorTexture
		? *nextImageIndex
		: defaultColorIndex;

	if (containsColorTexture) {
		const auto* colorView = model.GetTextureTable().GetImageView(material.colorTextureIndex.value());

		(*nextImageIndex)++;

		m_gdrMaterialInstance->GetSlot("images")->SetGpuImage(
			"images", 
			*colorView, 
			GpuImageSamplerDesc::CreateDefault_WithMipMap(colorView->GetImage()),
			colorViewIndex);
	}

	const UIndex32 normalViewIndex = containsNormalTexture
		? *nextImageIndex
		: defaultNormalIndex;

	if (containsNormalTexture) {
		const auto* normalView = model.GetTextureTable().GetImageView(material.normalTextureIndex.value());

		(*nextImageIndex)++;

		m_gdrMaterialInstance->GetSlot("images")->SetGpuImage(
			"images", 
			*normalView,
			GpuImageSamplerDesc::CreateDefault_WithMipMap(normalView->GetImage()),
			normalViewIndex);
	}


	GdrPerMaterialInfo materialInfo{};
	materialInfo.albedoTextureIndex = colorViewIndex;
	materialInfo.normalTextureIndex = normalViewIndex;
	materialInfo.metallicRoughness = Vector2f(
		material.metallicFactor,
		material.roughnessFactor
	);
	materialInfo.emissiveColor = material.emissiveColor;

	m_perMaterialTable[resourceIndex]->Write(materialInfo);

	previousOffsets->materialOffset++;
}

nlohmann::json GdrDeferredRenderSystem::SaveConfiguration() const {
	OSK_ASSERT(false, NotImplementedException());
}

PERSISTENCE::BinaryBlock GdrDeferredRenderSystem::SaveBinaryConfiguration() const {
	OSK_ASSERT(false, NotImplementedException());
}

void GdrDeferredRenderSystem::ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) {
	OSK_ASSERT(false, NotImplementedException());
}

void GdrDeferredRenderSystem::ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) {

}
