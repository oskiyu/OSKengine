#include "PreBuiltSplineLoader3D.h"

#include "FileIO.h"
#include "ModelLoadingExceptions.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include <set>

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::ASSETS;


void PreBuiltSplineLoader3D::Load(const std::string& assetFilePath, PreBuiltSpline3D* asset) {
	const nlohmann::json assetInfo = nlohmann::json::parse(OSK::IO::FileIO::ReadFromFile(assetFilePath));

	asset->SetName(assetInfo["name"]);

	m_loaded = asset;

	m_modelTransform = glm::scale(m_modelTransform, glm::vec3(assetInfo["scale"]));

	if (assetInfo.contains("rotation_offset")) {
		m_modelTransform = glm::rotate(m_modelTransform, glm::radians((float)assetInfo["rotation_offset"][0]), { 1.0f, 0.0f, 0.0f });
		m_modelTransform = glm::rotate(m_modelTransform, glm::radians((float)assetInfo["rotation_offset"][1]), { 0.0f, 1.0f, 0.0f });
		m_modelTransform = glm::rotate(m_modelTransform, glm::radians((float)assetInfo["rotation_offset"][2]), { 0.0f, 0.0f, 1.0f });
	}

	tinygltf::TinyGLTF context;

	std::string errorMessage = "";
	std::string warningMessage = "";

	tinygltf::Model gltfModel{};
	context.LoadBinaryFromFile(&gltfModel, &errorMessage, &warningMessage, assetInfo["raw_asset_path"]);
	OSK_ASSERT(errorMessage.empty(), EngineException("Error al cargar modelo estático: " + errorMessage));

	// Escena con los colliders
	const tinygltf::Scene scene = gltfModel.scenes[0];

	// Cada nodo representa un collider de bajo nivel.
	for (const int nodeIndex : scene.nodes) {
		const tinygltf::Node& node = gltfModel.nodes[nodeIndex];
		constexpr auto noParentId = std::numeric_limits<UIndex32>::max();

		ProcessNode(gltfModel, node, nodeIndex, noParentId);
	}

	m_loaded->_Get().Normalize();
}

void PreBuiltSplineLoader3D::ProcessNode(const tinygltf::Model& model, const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) {
	const glm::mat4 nodeMatrix = m_modelTransform * GetNodeMatrix(node);

	if (node.mesh <= -1) {
		for (UIndex32 i = 0; i < node.children.size(); i++) {
			ProcessNode(model, model.nodes[node.children[i]], node.children[i], parentId);
		}

		return;
	}

	// Mesh
	OSK::Engine::GetLogger()->InfoLog(std::format("Loading spline mesh {}", node.mesh));
	const tinygltf::Mesh& mesh = model.meshes[node.mesh];

	for (UIndex32 i = 0; i < mesh.primitives.size(); i++) {
		OSK::Engine::GetLogger()->InfoLog(std::format("Loading spline primitive {}.{}", node.mesh, i));
		const tinygltf::Primitive& primitive = mesh.primitives[i];

		std::set<GRAPHICS::TIndexSize> visitedIndices{};

		OSK_ASSERT(
			primitive.mode == TINYGLTF_MODE_LINE ||
			primitive.mode == TINYGLTF_MODE_LINE_LOOP ||
			primitive.mode == TINYGLTF_MODE_LINE_STRIP,
			UnsupportedPolygonModeException(std::to_string(primitive.mode)));

		const auto positions = GetVertexPositions(primitive, nodeMatrix, model);
		const auto indices = GetIndices(primitive, 0, model);

#ifdef OSK_DEBUG_SPLINE_LOADING
		OSK::Engine::GetLogger()->InfoLog(std::format("\tNum. positions: {}", positions.GetSize()));
		OSK::Engine::GetLogger()->InfoLog(std::format("\tNum. indices: {}", indices.GetSize()));
		OSK::Engine::GetLogger()->InfoLog(std::format("\tMode: {}", primitive.mode));
#endif // OSK_DEBUG_SPLINE_LOADING

		for (UIndex64 ind = 0; ind < indices.GetSize(); ind++) {
#ifdef OSK_DEBUG_SPLINE_LOADING
			OSK::Engine::GetLogger()->InfoLog(std::format("\t\tIndex {} is {}", ind, indices[ind]));
			OSK::Engine::GetLogger()->InfoLog(std::format("\t\tPosition at index {} is {:.2f} {:.2f} {:.2f}", ind, positions[indices[ind]].x, positions[indices[ind]].y, positions[indices[ind]].z));
#endif // OSK_DEBUG_SPLINE_LOADING
			
			if (!visitedIndices.contains(indices[ind])) {
				m_loaded->_Get().AddPoint(positions[indices[ind]]);
				visitedIndices.insert(indices[ind]);
			}
		}
		/* for (UIndex64 v : indices) {
			m_loaded->_Get().AddPoint(positions[v]);
		}
		for (const auto& v : positions) {
			m_loaded->_Get().AddPoint(v);
		}*/
	}

	for (UIndex32 i = 0; i < node.children.size(); i++) {
		ProcessNode(model, model.nodes[node.children[i]], node.children[i], parentId);
	}
}
