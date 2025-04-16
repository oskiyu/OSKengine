#include "PreBuiltSplineLoader3D.h"

#include "FileIO.h"
#include "ModelLoadingExceptions.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include <set>

#include "OSKengine.h"
#include "Logger.h"
#include "Vertex.h"

using namespace OSK;
using namespace OSK::ASSETS;


void PreBuiltSplineLoader3D::Load(const std::string& assetFilePath, PreBuiltSpline3D* asset) {
	const nlohmann::json assetInfo = nlohmann::json::parse(OSK::IO::FileIO::ReadFromFile(assetFilePath));

	asset->SetName(assetInfo["name"]);

	auto modelTransform = glm::mat4(1.0f);
	modelTransform = glm::scale(modelTransform, glm::vec3(assetInfo["scale"]));

	if (assetInfo.contains("rotation_offset")) {
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][0]), { 1.0f, 0.0f, 0.0f });
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][1]), { 0.0f, 1.0f, 0.0f });
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][2]), { 0.0f, 0.0f, 1.0f });
	}


	const auto& model = GltfLoader::Load(assetInfo["raw_asset_path"], modelTransform);

	Spline3D& output = asset->_Get();

	for (const auto& mesh : model.GetMeshes()) {
		const auto& vertices = mesh.GetVertices();
		std::set<GRAPHICS::TIndexSize> visitedIndices{};

		for (const auto& line : mesh.GetLines()) {
			for (const auto index : line) {
				if (!visitedIndices.contains(static_cast<GRAPHICS::TIndexSize>(index))) {
					output.AddPoint(vertices[index].position.value());
					visitedIndices.insert(static_cast<GRAPHICS::TIndexSize>(index));
				}
			}
		}
	}

	output.Normalize();
}
