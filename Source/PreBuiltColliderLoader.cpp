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
	m_buildingCollider = collider.GetPointer();

	// Asset file.
	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string rawAssetPath = assetInfo["raw_asset_path"];
	asset->SetName(assetInfo["name"]);

	// Loading
	m_modelTransform = glm::mat4(1.0f);

	m_modelTransform = glm::scale(m_modelTransform, glm::vec3(assetInfo["scale"]));

	if (assetInfo.contains("rotation_offset")) {
		m_modelTransform = glm::rotate(m_modelTransform, glm::radians((float)assetInfo["rotation_offset"][0]), { 1.0f, 0.0f, 0.0f });
		m_modelTransform = glm::rotate(m_modelTransform, glm::radians((float)assetInfo["rotation_offset"][1]), { 0.0f, 1.0f, 0.0f });
		m_modelTransform = glm::rotate(m_modelTransform, glm::radians((float)assetInfo["rotation_offset"][2]), { 0.0f, 0.0f, 1.0f });
	}

	tinygltf::TinyGLTF context;

	std::string errorMessage = "";
	std::string warningMessage = "";

	// Carga de archivo GLTF.
	tinygltf::Model gltfModel{};

	context.LoadBinaryFromFile(&gltfModel, &errorMessage, &warningMessage, rawAssetPath);
	OSK_ASSERT(errorMessage.empty(), EngineException("Error al cargar modelo estático: " + errorMessage));

	// Escena con los colliders
	const tinygltf::Scene scene = gltfModel.scenes[0];

	// Cada nodo representa un collider de bajo nivel.
	for (const int nodeIndex : scene.nodes) {
		const tinygltf::Node& node = gltfModel.nodes[nodeIndex];
		constexpr auto noParentId = std::numeric_limits<UIndex32>::max();

		ProcessNode(gltfModel, node, nodeIndex, noParentId);
	}

	// Top-level collider:
	Vector3f center = Vector3f::Zero;
	USize64 count = 0;

	for (UIndex64 i = 0; i < m_buildingCollider->GetBottomLevelCollidersCount(); i++) {
		const auto& blc = m_buildingCollider->GetBottomLevelCollider(i);

		for (const auto& vertex : blc->As<ConvexVolume>()->GetLocalSpaceVertices()) {
			center += vertex;
			count++;
		}
	}

	center /= static_cast<float>(count);

	// Radio
	float radius = 0.0f;

	for (UIndex64 i = 0; i < m_buildingCollider->GetBottomLevelCollidersCount(); i++) {
		auto* blc = m_buildingCollider->GetBottomLevelCollider(i)->As<ConvexVolume>();
		blc->MergeFaces();

		for (const auto& vertex : blc->GetLocalSpaceVertices()) {
			const float distance2 = vertex.GetDistanceTo2(center);
			radius = glm::max(radius, distance2);
		}
	}

	radius = glm::sqrt(radius);

	m_buildingCollider->SetTopLevelCollider(new SphereCollider(radius));
	
	asset->_SetCollider(collider);
	m_buildingCollider = nullptr;
}

void PreBuiltColliderLoader::ProcessNode(const tinygltf::Model& model, const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) {
	const glm::mat4 nodeMatrix = m_modelTransform * GetNodeMatrix(node);

	if (node.mesh <= -1) {
		for (UIndex32 i = 0; i < node.children.size(); i++)
			ProcessNode(model, model.nodes[node.children[i]], node.children[i], parentId);

		return;
	}

	// Mesh
	const tinygltf::Mesh& mesh = model.meshes[node.mesh];

	for (UIndex32 i = 0; i < mesh.primitives.size(); i++) {
		OwnedPtr<ConvexVolume> volume = new ConvexVolume();

		const tinygltf::Primitive& primitive = mesh.primitives[i];

		OSK_ASSERT(primitive.mode == TINYGLTF_MODE_TRIANGLES, UnsupportedPolygonModeException(std::to_string(primitive.mode)));

		const auto positions = GetVertexPositions(primitive, nodeMatrix, model);
		const auto indices = GetIndices(primitive, 0, model);

		const auto numIndices = indices.GetSize();

		// Procesamos los buffers y generamos nuevos vértices.
		for (UIndex64 ni = 0; ni < numIndices; ni += 3) {
			volume->AddFace({
				positions[indices[ni + 0]],
				positions[indices[ni + 1]],
				positions[indices[ni + 2]]
				});
		}

		volume->MergeFaces();

		m_buildingCollider->AddBottomLevelCollider(volume.GetPointer());
	}

	for (UIndex32 i = 0; i < node.children.size(); i++)
		ProcessNode(model, model.nodes[node.children[i]], node.children[i], parentId);
}
