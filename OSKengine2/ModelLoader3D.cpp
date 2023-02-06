#include "ModelLoader3D.h"

#include "Model3D.h"
#include "DynamicArray.hpp"
#include "Vertex.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuVertexBuffer.h"
#include "IGpuIndexBuffer.h"
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

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/matrix_decompose.hpp>
#include <gtc/type_ptr.hpp>
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

void ModelLoader3D::Load(const std::string& assetFilePath, IAsset** asset) {
	Model3D* output = (Model3D*)*asset;

	// a
	nlohmann::json assetInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(assetFilePath));

	OSK_ASSERT(assetInfo.contains("file_type"), "Archivo de modelo 3D incorrecto: no se encuentra 'file_type'.");
	OSK_ASSERT(assetInfo.contains("spec_ver"), "Archivo de modelo 3D incorrecto: no se encuentra 'spec_ver'.");
	OSK_ASSERT(assetInfo.contains("name"), "Archivo de modelo 3D incorrecto: no se encuentra 'name'.");
	OSK_ASSERT(assetInfo.contains("asset_type"), "Archivo de modelo 3D incorrecto: no se encuentra 'asset_type'.");
	OSK_ASSERT(assetInfo.contains("raw_asset_path"), "Archivo de modelo 3D incorrecto: no se encuentra 'faces_files'.");
	OSK_ASSERT(assetInfo.contains("scale"), "Archivo de modelo 3D incorrecto: no se encuentra 'scale'.");

	std::string texturePath = assetInfo["raw_asset_path"];
	output->SetName(assetInfo["name"]);

	OSK_ASSERT(IO::FileIO::FileExists(assetInfo["raw_asset_path"]), "El modelo en no existe.");

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
	for (TSize i = 0; i < model.GetMeshes().GetSize(); i++) {
		auto& meshMetadata = model.GetMetadata().meshesMetadata[i];

		if (meshMetadata.materialTextures.GetSize() > 0)
			for (auto& [_, texture] : meshMetadata.materialTextures)
				component->GetMeshMaterialInstance(i)->GetSlot("texture")->SetGpuImage("albedoTexture", model.GetImage(texture));

		component->GetMeshMaterialInstance(i)->GetSlot("texture")->FlushUpdate();
	}
}
