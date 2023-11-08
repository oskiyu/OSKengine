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


void ModelLoader3D::Load(const std::string& assetFilePath, IAsset** asset) {
	Model3D* output = (Model3D*)*asset;

	// a
	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string modelPath = assetInfo["raw_asset_path"];
	output->SetName(assetInfo["name"]);

	OSK_ASSERT(assetInfo.contains("renderpass_type"), InvalidDescriptionFileException("No se encuentra renderpass_type", assetFilePath));

	output->SetRenderPassType(assetInfo["renderpass_type"]);

	const bool isAnimated = assetInfo.contains("animated");

	auto modelTransform = glm::mat4(1.0f);

	modelTransform = glm::scale(modelTransform, glm::vec3(assetInfo["scale"]));

	if (assetInfo.contains("rotation_offset")) {
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][0]), { 1.0f, 0.0f, 0.0f});
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][1]), { 0.0f, 1.0f, 0.0f });
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][2]), { 0.0f, 0.0f, 1.0f });
	}

	/*for (const auto& lodInfo : assetInfo["lods"]) {
		const std::string_view path = lodInfo["path"];


	}*/

	const USize64 prevNumMeshes = output->GetMeshes().GetSize();

	if (isAnimated) {
		AnimMeshLoader loader{};
		loader.Load(modelPath, modelTransform);
		loader.SetupModel(output);
	}
	else {
		StaticMeshLoader loader{};
		loader.Load(modelPath, modelTransform);
		loader.SetupModel(output);
	}

	const USize64 newNumMeshes = output->GetMeshes().GetSize();

	output->_RegisterLod(Model3D::Lod{ .firstMeshId = prevNumMeshes, .meshesCount = newNumMeshes - prevNumMeshes });

	output->_SetId(m_nextModelId);
	m_nextModelId++;
}
