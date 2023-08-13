#include "ModelLoader3D.h"

#include "Model3D.h"
#include "DynamicArray.hpp"
#include "Vertex.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuBuffer.h"
#include "IGpuMemoryAllocator.h"
#include "FileIO.h"
#include "Mesh3D.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "IGpuImage.h"
#include "GpuImageLayout.h"
#include "OwnedPtr.h"
#include "GpuImageUsage.h"
#include "GpuImageDimensions.h"
#include "Color.hpp"
#include "Vertex3D.h"
#include "ModelComponent3D.h"
#include "Texture.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <json.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>
#undef TINYGLTF_IMPLEMENTATION
#undef STB_IMAGE_IMPLEMENTATION
#undef TINYGLTF_NO_STB_IMAGE_WRITE

#include "StaticMeshLoader.h"
#include "AnimMeshLoader.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

ModelLoader3D::ModelLoader3D() {
	SetupDefaultNormalTexture();
}

ModelLoader3D::~ModelLoader3D() {
	defaultNormalTexture.Delete();
}

void ModelLoader3D::SetupDefaultNormalTexture() {
	IRenderer* renderer = Engine::GetRenderer();
	IGpuMemoryAllocator* mAllocator = renderer->GetAllocator();
	OwnedPtr<ICommandList> uploadCmdList = renderer->CreateSingleUseCommandList();
	uploadCmdList->Start();

	TByte data[4] = {
		127, // R: x
		127, // G: y
		255, // B: z
		255	 // A
	};

	OwnedPtr<GpuBuffer> stagingBuffer = mAllocator->CreateStagingBuffer(sizeof(data), GpuBufferUsage::TRANSFER_SOURCE);
	stagingBuffer->MapMemory();
	stagingBuffer->Write(data, sizeof(data));
	stagingBuffer->Unmap();

	GpuImageCreateInfo imageInfo = GpuImageCreateInfo::CreateDefault2D(
		Vector2ui(1, 1),
		Format::RGBA8_UNORM,
		GpuImageUsage::SAMPLED | GpuImageUsage::TRANSFER_DESTINATION);
	imageInfo.samplerDesc.mipMapMode = GpuImageMipmapMode::NONE;
	imageInfo.samplerDesc.addressMode = GpuImageAddressMode::REPEAT;
	defaultNormalTexture = mAllocator->CreateImage(imageInfo).GetPointer();

	uploadCmdList->SetGpuImageBarrier(
		defaultNormalTexture.GetPointer(),
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	uploadCmdList->CopyBufferToImage(stagingBuffer.GetValue(), defaultNormalTexture.GetPointer());

	uploadCmdList->SetGpuImageBarrier(
		defaultNormalTexture.GetPointer(),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SHADER_READ));
	
	uploadCmdList->Close();

	renderer->SubmitSingleUseCommandList(uploadCmdList);
}

void ModelLoader3D::Load(const std::string& assetFilePath, IAsset** asset) {
	Model3D* output = (Model3D*)*asset;

	// a
	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string texturePath = assetInfo["raw_asset_path"];
	output->SetName(assetInfo["name"]);

	const bool isAnimated = assetInfo.contains("animated");

	glm::mat4 modelTransform = glm::mat4(1.0f);

	modelTransform = glm::scale(modelTransform, glm::vec3(assetInfo["scale"]));

	if (assetInfo.contains("rotation_offset")) {
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][0]), { 1.0f, 0.0f, 0.0f});
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][1]), { 0.0f, 1.0f, 0.0f });
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][2]), { 0.0f, 0.0f, 1.0f });
	}


	if (isAnimated) {
		AnimMeshLoader loader{};
		loader.Load(assetInfo["raw_asset_path"], modelTransform);
		loader.SetupModel(output);
	}
	else {
		StaticMeshLoader loader{};
		loader.Load(assetInfo["raw_asset_path"], modelTransform);
		loader.SetupModel(output);
	}
}

void ModelLoader3D::SetupPbrModel(const Model3D& model, ECS::ModelComponent3D* component) {
	const GpuImageViewConfig view = GpuImageViewConfig::CreateSampled_Default();

	const auto defaultNormalTextureView = defaultNormalTexture->GetView(view);
	const auto defaultTextureView = Engine::GetAssetManager()->Load<Texture>("Resources/Assets/Textures/texture0.json", "GLOBAL")
		->GetGpuImage()->GetView(view);
	
	// Texturas por defecto
	for (UIndex32 i = 0; i < model.GetMeshes().GetSize(); i++) {
		component->GetMeshMaterialInstance(i)->GetSlot("texture")->SetGpuImage("albedoTexture", defaultTextureView);
		component->GetMeshMaterialInstance(i)->GetSlot("texture")->SetGpuImage("normalTexture", defaultNormalTextureView);
	}

	for (UIndex32 i = 0; i < model.GetMeshes().GetSize(); i++) {
		const auto& meshMetadata = model.GetMetadata().meshesMetadata[i];
		
		if (meshMetadata.materialTextures.size() > 0) {
			for (auto& [name, texture] : meshMetadata.materialTextures) {
				if (name == "baseTexture")
					component->GetMeshMaterialInstance(i)->GetSlot("texture")->SetGpuImage("albedoTexture", model.GetImage(texture)->GetView(view));
				else if (name == "normalTexture")
					component->GetMeshMaterialInstance(i)->GetSlot("texture")->SetGpuImage("normalTexture", model.GetImage(texture)->GetView(view));
			}
		}

		component->GetMeshMaterialInstance(i)->GetSlot("texture")->FlushUpdate();
	}
}

UniquePtr<GpuImage> ModelLoader3D::defaultNormalTexture = nullptr;
