#include "IRenderPass.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"

#include "AssetManager.h"

#include "Model3D.h"
#include "Mesh3D.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void IRenderPass::SetCamera(ECS::GameObjectIndex cameraObject) {
	m_cameraObject = cameraObject;
}

std::string_view IRenderPass::GetTypeName() const {
	return m_passTypeName;
}

void IRenderPass::SetupMaterialInstance(const Model3D& model, const Mesh3D& mesh) {
	m_meshMapping.RegisterModel(model.GetId());

	auto& modelData = m_meshMapping.GetModelData(model.GetId());
	modelData.RegisterMesh(mesh.GetMeshId());

	auto& meshData = modelData.GetMeshData(mesh.GetMeshId());

	meshData._SetMaterialInstance(m_passMaterial->CreateInstance().GetPointer());

	// Material Instance
	const GpuImageViewConfig view = GpuImageViewConfig::CreateSampled_Default();

	const auto defaultNormalTextureView = Engine::GetRenderer()->GetAllocator()->GetDefaultNormalTexture()->GetView(view);
	defaultTexture = Engine::GetAssetManager()->Load<Texture>("Resources/Assets/Textures/texture0.json");
	const auto defaultTextureView = defaultTexture->GetGpuImage()->GetView(view);

	auto* mInstance = meshData.GetMaterialInstance();

	const auto& meshMetadata = model.GetMetadata().meshesMetadata[mesh.GetMeshId()];

	const auto& textureTable = meshMetadata.textureTable;
	const auto* colorTexture = textureTable.contains(ModelMetadata::BaseColorTextureName)
		? model.GetMetadata().textures[textureTable.find(ModelMetadata::BaseColorTextureName)->second]->GetView(view)
		: defaultTextureView;
	const auto* normalTexture = textureTable.contains(ModelMetadata::NormalTextureName)
		? model.GetMetadata().textures[textureTable.find(ModelMetadata::NormalTextureName)->second]->GetView(view)
		: defaultNormalTextureView;

	const auto* materialBuffer = model.GetMetadata().materialInfos[meshMetadata.materialId].GetPointer();

	mInstance->GetSlot("texture")->SetGpuImage("albedoTexture", colorTexture);
	mInstance->GetSlot("texture")->SetGpuImage("normalTexture", normalTexture);
	mInstance->GetSlot("texture")->SetUniformBuffer("materialInfo", *materialBuffer);
	mInstance->GetSlot("texture")->FlushUpdate();
}
