#include "IShaderPass.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"

#include "AssetManager.h"

#include "Model3D.h"
#include "PbrMaterialInfo.h"

#include "ModelComponent3D.h"


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;



void IShaderPass::UpdateLocalMeshMapping(const DynamicArray<ECS::GameObjectIndex>& objectsToRender) {
	for (const auto& obj : objectsToRender) {
		const auto* model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj).GetModel();

		if (!m_localMeshMapping.HasModel(model->GetUuid())) {
			m_localMeshMapping.RegisterModel(model->GetUuid());
		}

		auto& modelData = m_localMeshMapping.GetModelData(model->GetUuid());

		for (const auto& mesh : model->GetMeshes()) {
			if (!modelData.HasMesh(mesh.GetUuid())) {
				modelData.RegisterMesh(mesh.GetUuid());
				SetupMaterialInstance(*model, mesh);
			}
		}
	}
}

void IShaderPass::SetCamera(ECS::GameObjectIndex cameraObject) {
	m_cameraObject = cameraObject;
}

std::string_view IShaderPass::GetTypeName() const {
	return m_passTypeName;
}

void IShaderPass::SetupMaterialInstance(const GpuModel3D& model, const GpuMesh3D& mesh) {
	m_localMeshMapping.RegisterModel(model.GetUuid());

	auto& modelData = m_localMeshMapping.GetModelData(model.GetUuid());
	modelData.RegisterMesh(mesh.GetUuid());

	auto& meshData = modelData.GetMeshData(mesh.GetUuid());

	meshData._SetMaterialInstance(m_passMaterial->CreateInstance().GetPointer());
	meshData._SetMaterialBuffer(Engine::GetRenderer()->GetAllocator()->CreateBuffer(sizeof(PbrMaterialInfo), 0, GpuBufferUsage::UNIFORM_BUFFER, GpuSharedMemoryType::GPU_AND_CPU));

	// Material Instance
	const GpuImageViewConfig view = GpuImageViewConfig::CreateSampled_Default();

	const auto defaultNormalTextureView = Engine::GetRenderer()->GetAllocator()->GetDefaultNormalTexture()->GetView(view);
	const auto defaultTexture = Engine::GetAssetManager()->Load<Texture>("Resources/Assets/Textures/texture0.json");
	const auto defaultTextureView = defaultTexture->GetGpuImage()->GetView(view);

	auto* mInstance = meshData.GetMaterialInstance();

	const auto& material = model.GetMaterial(mesh.GetMaterialIndex());
	
	const auto& textureTable = model.GetTextureTable();
	
	const auto* colorTexture = material.colorTextureIndex.has_value()
		? textureTable.GetImageView(material.colorTextureIndex.value())
		: defaultTextureView;
	const auto* normalTexture = material.normalTextureIndex.has_value()
		? textureTable.GetImageView(material.normalTextureIndex.value())
		: defaultNormalTextureView;

	const auto* materialBuffer = meshData.GetMaterialBuffer();

	mInstance->GetSlot("texture")->SetGpuImage("albedoTexture", colorTexture);
	mInstance->GetSlot("texture")->SetGpuImage("normalTexture", normalTexture);
	mInstance->GetSlot("texture")->SetUniformBuffer("materialInfo", *materialBuffer);
	mInstance->GetSlot("texture")->FlushUpdate();

	PbrMaterialInfo materialInfo{};
	materialInfo.emissiveColor = material.emissiveColor;
	materialInfo.roughnessMetallic = { material.roughnessFactor, material.metallicFactor };
	materialInfo.hasNormalTexture = material.normalTextureIndex.has_value();

	materialBuffer->GetMemorySubblock()->MapMemory();
	materialBuffer->GetMemorySubblock()->Write(&materialInfo, sizeof(materialInfo));
	materialBuffer->GetMemorySubblock()->Unmap();
}


// --- ShaderPassTable --- //

void ShaderPassTable::AddShaderPass(OwnedPtr<IShaderPass> pass) {
	m_renderPasses.Insert(pass.GetPointer());
	m_renderPassesPtrs.Insert(pass.GetPointer());

	m_renderPassesTable[(std::string)pass->GetTypeName()] = pass.GetPointer();
	m_objectsPerPass[(std::string)pass->GetTypeName()] = {};
}

void ShaderPassTable::RemoveShaderPass(std::string_view passName) {
	// todo: delete

	for (UIndex64 i = 0; i < m_renderPassesPtrs.GetSize(); i++) {
		if (m_renderPasses[i]->GetTypeName() == passName) {
			m_renderPasses.RemoveAt(i);
			break;
		}
	}

	const auto strName = static_cast<std::string>(passName);
	m_renderPassesTable.erase(strName);
	m_objectsPerPass.erase(strName);
}

bool ShaderPassTable::ContainsShaderPass(std::string_view passName) const {
	return m_renderPassesTable.contains(passName);
}

IShaderPass* ShaderPassTable::GetShaderPass(std::string_view passName) {
	return m_renderPassesTable.find(passName)->second;
}
const IShaderPass* ShaderPassTable::GetShaderPass(std::string_view passName) const {
	return m_renderPassesTable.find(passName)->second;
}


void ShaderPassTable::AddCompatibleObject(std::string_view passName, GameObjectIndex obj) {
	m_objectsPerPass.find(passName)->second.Insert(obj);
}

void ShaderPassTable::RemoveCompatibleObject(std::string_view passName, GameObjectIndex obj) {
	m_objectsPerPass.find(passName)->second.Remove(obj);
}

DynamicArray<GameObjectIndex>& ShaderPassTable::GetCompatibleObjects(std::string_view passName) {
	return m_objectsPerPass.find(passName)->second;
}

const DynamicArray<GameObjectIndex>& ShaderPassTable::GetCompatibleObjects(std::string_view passName) const {
	return m_objectsPerPass.find(passName)->second;
}

DynamicArray<GRAPHICS::IShaderPass*>& ShaderPassTable::GetAllPasses() {
	return m_renderPassesPtrs;
}
