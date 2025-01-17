#include "PreBuiltColliderLoader.h"

#include "PreBuiltCollider.h"
#include "ConvexVolume.h"
#include "ModelLoadingExceptions.h"
#include "SphereCollider.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>


using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::COLLISION;

void PreBuiltColliderLoader::Load(const std::string& assetFilePath, PreBuiltCollider* asset) {
	OwnedPtr<COLLISION::Collider> collider = new COLLISION::Collider();

	// Asset file.
	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string rawAssetPath = assetInfo["raw_asset_path"];
	asset->SetName(assetInfo["name"]);

	// Loading
	auto modelTransform = glm::mat4(1.0f);

	modelTransform = glm::scale(modelTransform, glm::vec3(assetInfo["scale"]));

	if (assetInfo.contains("rotation_offset")) {
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][0]), { 1.0f, 0.0f, 0.0f });
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][1]), { 0.0f, 1.0f, 0.0f });
		modelTransform = glm::rotate(modelTransform, glm::radians((float)assetInfo["rotation_offset"][2]), { 0.0f, 0.0f, 1.0f });
	}

	const CpuModel3D model = GltfLoader::Load(rawAssetPath, modelTransform, 1.0f);

	for (const auto& mesh : model.GetMeshes()) {
		const auto& vertices = mesh.GetVertices();

		OwnedPtr<ConvexVolume> volume = new ConvexVolume();

		for (const auto& triangle : mesh.GetTriangles()) {
			volume->AddFace({ 
				vertices[triangle[0]].position.value(),
				vertices[triangle[1]].position.value(),
				vertices[triangle[2]].position.value() });
		}

		volume->MergeFaces();

		collider->AddBottomLevelCollider(volume.GetPointer());
	}

	// Top-level collider:
	Vector3f center = Vector3f::Zero;
	USize32 count = 0;

	for (UIndex32 i = 0; i < collider->GetBottomLevelCollidersCount(); i++) {
		const auto& blc = collider->GetBottomLevelCollider(i);

		for (const auto& vertex : blc->As<ConvexVolume>()->GetLocalSpaceVertices()) {
			center += vertex;
			count++;
		}
	}

	center /= static_cast<float>(count);

	// Radio
	float radius = 0.0f;

	for (UIndex64 i = 0; i < collider->GetBottomLevelCollidersCount(); i++) {
		auto* blc = collider->GetBottomLevelCollider(i)->As<ConvexVolume>();
		blc->MergeFaces();

		for (const auto& vertex : blc->GetLocalSpaceVertices()) {
			const float distance2 = vertex.GetDistanceTo2(center);
			radius = glm::max(radius, distance2);
		}
	}

	radius = glm::sqrt(radius);

	collider->SetTopLevelCollider(new SphereCollider(radius));
	
	asset->_SetCollider(collider);
}
