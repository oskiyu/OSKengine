#include "ModelLoader3D.h"

#include "Model3D.h"
#include "DynamicArray.hpp"
#include "Vertex.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuBuffer.h"
#include "IGpuMemoryAllocator.h"
#include "FileIO.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "IGpuImage.h"
#include "GpuImageLayout.h"
#include "GpuImageUsage.h"
#include "GpuImageDimensions.h"
#include "Color.hpp"
#include "Vertex3D.h"
#include "ModelComponent3D.h"
#include "Texture.h"

#include "InvalidDescriptionFileException.h"

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


void ModelLoader3D::Load(const std::string& assetFilePath, Model3D* asset) {
	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string modelPath = assetInfo["raw_asset_path"];
	asset->SetName(assetInfo["name"]);

	OSK_ASSERT(assetInfo.contains("renderpass_type"), InvalidDescriptionFileException("No se encuentra renderpass_type", assetFilePath));

	const bool isAnimated = assetInfo.contains("animated");

	auto modelTransform = glm::mat4(1.0f);

	modelTransform = glm::scale(modelTransform, glm::vec3(assetInfo["scale"]));

	if (assetInfo.contains("rotation_offset")) {
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][0]), { 1.0f, 0.0f, 0.0f });
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][1]), { 0.0f, 1.0f, 0.0f });
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][2]), { 0.0f, 0.0f, 1.0f });
	}

	auto cpuModel = GltfLoader::Load(
		modelPath,
		modelTransform);

	if (isAnimated) {
		AnimMeshLoader loader{};
		loader.Load(cpuModel, &asset->_GetModel());
	}
	else {
		StaticMeshLoader loader{};
		loader.Load(cpuModel, &asset->_GetModel());
	}

	DynamicArray<GpuModel3D::Material> materials{};
	GltfLoader::LoadMaterials(
		modelPath,
		&materials,
		&asset->_GetModel().GetTextureTable());

	asset->_GetModel().SetMaterials(materials);

	asset->_GetModel().SetUuid(GpuModelUuid(Engine::GetUuidProvider()->GenerateNewUuid()));
}
