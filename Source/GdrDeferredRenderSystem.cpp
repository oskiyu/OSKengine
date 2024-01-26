#include "GdrDeferredSystem.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"

#include "ModelComponent3D.h"
#include "Transform3D.h"

#include "Vertex3D.h"
#include "VertexAttributes.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;


GdrDeferredRenderSystem::GdrDeferredRenderSystem() : DeferredRenderSystem() {
}

void GdrDeferredRenderSystem::SetMaxCounts(USize32 maxVertexCount, USize32 maxMeshCount) {
	CreateUnifiedBuffers(maxVertexCount, maxMeshCount);
}

void GdrDeferredRenderSystem::CreateUnifiedBuffers(USize32 maxVertexCount, USize32 maxMeshCount) {
	auto* allocator = Engine::GetRenderer()->GetAllocator();

	for (UIndex64 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		m_unifiedVertexBuffers[i] = allocator->CreateBuffer(sizeof(GdrVertex3D) * maxVertexCount, 0, GpuBufferUsage::VERTEX_BUFFER, GpuSharedMemoryType::GPU_AND_CPU).GetPointer();
		m_unifiedIndexBuffers[i] = allocator->CreateBuffer(sizeof(TIndexSize) * maxVertexCount * 3, 0, GpuBufferUsage::INDEX_BUFFER, GpuSharedMemoryType::GPU_AND_CPU).GetPointer();

		// Attributes
		m_vertexPositionsBuffers[i] = allocator->CreateStorageBuffer(sizeof(VertexPositionAttribute3D) * maxVertexCount).GetPointer();
		m_vertexAttributesBuffers[i] = allocator->CreateStorageBuffer(sizeof(VertexAttributes3D) * maxVertexCount).GetPointer();
		m_vertexAnimAttributesBuffers[i] = allocator->CreateStorageBuffer(sizeof(VertexAnimationAttributes3D) * maxVertexCount).GetPointer();

		// Tables
		m_perMaterialTable[i] = allocator->CreateStorageBuffer(sizeof(GdrPerMeshInfo) * maxMeshCount).GetPointer();
		m_perMeshTable[i] = allocator->CreateStorageBuffer(sizeof(GdrPerMaterialInfo) * maxMeshCount).GetPointer();
	}

	// Material
	m_gdrMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/GpuDriven/gdr_gbuffer.json");
	m_gdrMaterialInstance = m_gdrMaterial->CreateInstance().GetPointer();


	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> cameraUbos{};
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> previousCameraUbos{};

	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> meshTable{};
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> positionsTable{};
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> attributesTable{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		cameraUbos[i] = m_cameraBuffers[i].GetPointer();
		previousCameraUbos[i] = m_previousFrameCameraBuffers[i].GetPointer();

		meshTable[i] = m_perMeshTable[i].GetPointer();
		positionsTable[i] = m_vertexPositionsBuffers[i].GetPointer();
		attributesTable[i] = m_vertexAttributesBuffers[i].GetPointer();
	}

	m_gdrMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", cameraUbos);
	m_gdrMaterialInstance->GetSlot("global")->SetUniformBuffers("previousCamera", previousCameraUbos);
	m_gdrMaterialInstance->GetSlot("global")->FlushUpdate();

	m_gdrMaterialInstance->GetSlot("tables")->SetUniformBuffers("meshInfos", meshTable);
	m_gdrMaterialInstance->GetSlot("tables")->SetUniformBuffers("positions", positionsTable);
	m_gdrMaterialInstance->GetSlot("tables")->SetUniformBuffers("attributes", attributesTable);
	m_gdrMaterialInstance->GetSlot("tables")->FlushUpdate();
}

void GdrDeferredRenderSystem::Render(GRAPHICS::ICommandList* commandList) {
	const auto resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	m_unifiedVertexBuffers[resourceIndex]->ResetCursor();
	m_unifiedIndexBuffers[resourceIndex]->ResetCursor();

	m_vertexPositionsBuffers[resourceIndex]->ResetCursor();
	m_vertexAttributesBuffers[resourceIndex]->ResetCursor();
	m_vertexAnimAttributesBuffers[resourceIndex]->ResetCursor();

	m_perMeshTable[resourceIndex]->ResetCursor();
	m_perMaterialTable[resourceIndex]->ResetCursor();


	m_unifiedVertexBuffers[resourceIndex]->MapMemory();
	m_unifiedIndexBuffers[resourceIndex]->MapMemory();

	m_vertexPositionsBuffers[resourceIndex]->MapMemory();
	m_vertexAttributesBuffers[resourceIndex]->MapMemory();
	m_vertexAnimAttributesBuffers[resourceIndex]->MapMemory();

	m_perMeshTable[resourceIndex]->MapMemory();
	m_perMaterialTable[resourceIndex]->MapMemory();


	UIndex32 nextMeshIndex = 0;
	TIndexSize nextVertexIndex = 0;

	USize32 numVertices = 0;
	USize32 numIndices = 0;

	GdrPerMeshInfo previousOffsets{};

	UIndex32 nextImageIndex = 0;

	const UIndex32 defaultColorIndex = nextImageIndex;
	nextImageIndex++; // Default Color.

	const UIndex32 defaultNormalIndex = nextImageIndex;
	nextImageIndex++; // Default Normal.

	const auto* defaultNormalTexture = OSK::Engine::GetRenderer()->GetAllocator()->GetDefaultNormalTexture();
	m_gdrMaterialInstance->GetSlot("images")->SetGpuImage("images", defaultNormalTexture->GetView(GpuImageViewConfig::CreateSampled_Default()), defaultNormalIndex);
		


	const auto jitterIndex = m_taaProvider.GetCurrentFrameJitterIndex();
	const Vector2ui resolution = m_gBuffer.GetImage(GBuffer::Target::COLOR)->GetSize2D();

	std::unordered_map<const Mesh3D*, MeshDrawInfo> previousMeshesMap{};

	// TODO: passes
	for (GameObjectIndex obj : GetObjects()) {
		const ModelComponent3D& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);

		
		for (UIndex32 i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			const auto& mesh = model.GetModel()->GetMeshes()[i];
			// const bool isInsideFrustum = mesh.GetBounds().IsInsideFrustum(frustum);


			// Per mesh info

			GdrPerMeshInfo info{};
			info.positionsOffset = previousOffsets.positionsOffset;
			info.attributesOffset = previousOffsets.attributesOffset;
			info.animationAttributesOffset = previousOffsets.animationAttributesOffset;
			info.materialOffset = previousOffsets.materialOffset;
			info.modelMatrix = transform.GetAsMatrix();
			info.previousModelMatrix = m_previousFrameMatrices.contains(obj)
				? m_previousFrameMatrices.at(obj)
				: glm::mat4(1.0f);

			m_perMeshTable[resourceIndex]->Write(info);


			// Vertices IDs

			const USize32 nNumVertices = model.GetModel()->GetVertexBuffer()->GetVertexView().numVertices;
			const USize32 nNumIndices = model.GetModel()->GetIndexBuffer()->GetIndexView().numIndices;

			for (UIndex32 v = 0; v < nNumVertices; v++) {
				m_unifiedVertexBuffers[resourceIndex]->Write(GdrVertex3D{ .gdrIndex = nextMeshIndex });
			}

			for (UIndex32 idx = 0; idx < nNumIndices; idx++) {
				m_unifiedIndexBuffers[resourceIndex]->Write(nextVertexIndex + idx);
			}

			nextVertexIndex += nNumIndices;

			numVertices += nNumVertices;
			numIndices += nNumIndices;

			info.materialOffset++;


			// Vertices attributes

			const auto& positions = model.GetModel()->GetVerticesAttributes().GetVerticesAttributes<VertexPositionAttribute3D>();
			const auto& attributes = model.GetModel()->GetVerticesAttributes().GetVerticesAttributes<VertexAttributes3D>();

			previousOffsets.positionsOffset += static_cast<USize32>(positions.GetSize());
			previousOffsets.attributesOffset += static_cast<USize32>(attributes.GetSize());

			for (UIndex64 a = 0; a < positions.GetSize(); a++) {
				m_vertexPositionsBuffers[resourceIndex]->Write(positions[a]);
				m_vertexAttributesBuffers[resourceIndex]->Write(attributes[a]);
			}

			if (model.GetModel()->GetVerticesAttributes().HasAttribute(VertexAnimationAttributes3D::GetAttribName())) {
				const auto& animAttribs = model.GetModel()->GetVerticesAttributes().GetVerticesAttributes<VertexAttributes3D>();

				previousOffsets.animationAttributesOffset += static_cast<USize32>(animAttribs.GetSize());

				for (UIndex64 a = 0; a < animAttribs.GetSize(); a++) {
					m_vertexAnimAttributesBuffers[resourceIndex]->Write(animAttribs[a]);
				}
			}


			// Images

			const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_Default();

			const auto& modelMetadata = model.GetModel()->GetMetadata();

			const auto& meshMetadata = modelMetadata.meshesMetadata[i];

			const bool containsColorTexture = meshMetadata.textureTable.contains(ASSETS::ModelMetadata::BaseColorTextureName);
			const bool containsNormalTexture = meshMetadata.textureTable.contains(ASSETS::ModelMetadata::NormalTextureName);

			const UIndex32 colorViewIndex = containsColorTexture
				? nextImageIndex
				: defaultColorIndex;

			if (containsColorTexture) {
				const auto* colorTexture = modelMetadata.textures[meshMetadata.textureTable.find(ASSETS::ModelMetadata::BaseColorTextureName)->second].GetPointer();
				const auto* colorView = colorTexture->GetView(viewConfig);

				nextImageIndex++;

				m_gdrMaterialInstance->GetSlot("images")->SetGpuImage("images", colorView, colorViewIndex);
			}

			const UIndex32 normalViewIndex = containsNormalTexture
				? nextImageIndex
				: defaultNormalIndex;

			if (containsNormalTexture) {
				const auto* normalTexture = modelMetadata.textures[meshMetadata.textureTable.find(ASSETS::ModelMetadata::NormalTextureName)->second].GetPointer();
				const auto* normalView = normalTexture->GetView(viewConfig);

				nextImageIndex++;

				m_gdrMaterialInstance->GetSlot("images")->SetGpuImage("images", normalView, normalViewIndex);
			}


			GdrPerMaterialInfo materialInfo{};
			materialInfo.albedoTextureIndex = colorViewIndex;
			materialInfo.normalTextureIndex = normalViewIndex;
			materialInfo.metallicRoughness = Vector2f(
				modelMetadata.materials[meshMetadata.materialId].metallicFactor,
				modelMetadata.materials[meshMetadata.materialId].roughnessFactor
			);
			materialInfo.emissiveColor = modelMetadata.materials[meshMetadata.materialId].emissiveColor;

			m_perMaterialTable[resourceIndex]->Write(materialInfo);
		}

		m_previousFrameMatrices[obj] = transform.GetAsMatrix();
	}

	m_gdrMaterialInstance->GetSlot("images")->FlushUpdate();

	m_unifiedVertexBuffers[resourceIndex]->Unmap();
	m_unifiedIndexBuffers[resourceIndex]->Unmap();

	m_vertexPositionsBuffers[resourceIndex]->Unmap();
	m_vertexAttributesBuffers[resourceIndex]->Unmap();
	m_vertexAnimAttributesBuffers[resourceIndex]->Unmap();

	m_perMeshTable[resourceIndex]->Unmap();
	m_perMaterialTable[resourceIndex]->Unmap();


	// Rendering
	commandList->StartDebugSection("PBR-Bindless GBuffer", Color::Red);
	m_gBuffer.BeginRenderpass(commandList, Color::Black * 0.0f, false);

	SetupViewport(commandList);

	commandList->BindMaterial(*m_gdrMaterial);
	commandList->BindMaterialInstance(m_gdrMaterialInstance.GetValue());

	VertexBufferView vertexView{};
	vertexView.vertexInfo = GdrVertex3D::GetVertexInfo();
	vertexView.numVertices = numVertices;
	vertexView.offsetInBytes = 0;
	commandList->BindVertexBufferRange(m_unifiedVertexBuffers[resourceIndex].GetValue(), vertexView);

	IndexBufferView indexView{};
	indexView.type = IndexType::U32;
	indexView.offsetInBytes = 0;
	indexView.numIndices = numIndices;
	commandList->BindVertexBufferRange(m_unifiedIndexBuffers[resourceIndex].GetValue(), vertexView);

	struct {
		Vector3f info{};
	} pushConstants {
		.info = Vector3f(
			static_cast<float>(resolution.x),
			static_cast<float>(resolution.y),
			static_cast<float>(jitterIndex))
	};

	commandList->PushMaterialConstants("pushConstants", pushConstants);

	commandList->DrawSingleMesh(0, numIndices);

	commandList->EndGraphicsRenderpass();
	commandList->EndDebugSection();
}
