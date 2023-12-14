#include "StaticMeshLoader.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"

#include "ModelLoadingExceptions.h"

#include <limits>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>


using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void StaticMeshLoader::ProcessNode(const tinygltf::Model& model, const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) {
	const glm::mat4 nodeMatrix = m_modelTransform * GetNodeMatrix(node);
	const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(nodeMatrix)));

	if (node.mesh <= -1) {
		for (UIndex32 i = 0; i < node.children.size(); i++)
			ProcessNode(model, model.nodes[node.children[i]], node.children[i], parentId);

		return;
	}

	// Proceso del polígono.
	const tinygltf::Mesh& mesh = model.meshes[node.mesh];

	for (UIndex32 i = 0; i < mesh.primitives.size(); i++) {
		const tinygltf::Primitive& primitive = mesh.primitives[i];

		if (primitive.material > -1)
			m_meshIdToMaterialId[static_cast<UIndex32>(m_meshes.GetSize())] = primitive.material;

		OSK_ASSERT(primitive.mode == TINYGLTF_MODE_TRIANGLES, UnsupportedPolygonModeException(std::to_string(primitive.mode)));

		const UIndex64 firstVertexId = vertices.GetSize();
		const UIndex64 firstIndexId = m_indices.GetSize();

		const auto primitiveIndices = GetIndices(primitive, firstVertexId, model);

		const auto positions = GetVertexPositions(primitive, nodeMatrix, model);
		const auto normals = GetVertexNormals(primitive, model);
		const auto texCoords = GetTextureCoords(primitive, model);
		const auto colors = GetVertexColors(primitive, model);


		const auto tangents = HasTangets(primitive)
			? GetTangentVectors(primitive, model)
			: GenerateTangetVectors(texCoords, positions, primitiveIndices, firstVertexId);

		const auto numVertices = positions.GetSize();

		const bool hasColors = !colors.IsEmpty();

		// Procesamos los buffers y generamos nuevos vértices.
		for (UIndex64 v = 0; v < numVertices; v++) {

			Vertex3D vertex{};
			vertex.position = positions[v];
			vertex.normal = Vector3f(glm::normalize(normalMatrix * normals[v].ToGlm()));
			vertex.texCoords = texCoords[v];
			vertex.color = Color::White;

			vertex.tangent = Vector3f(glm::normalize(normalMatrix * tangents[v].ToGlm()));

			if (primitive.material > -1)
				vertex.color = m_modelInfo.materialInfos[primitive.material].baseColor;

			if (hasColors)
				vertex.color = colors[v];

			vertices.Insert(vertex);
		}

		m_indices.InsertAll(primitiveIndices);

		// Bounding sphere.
		float radius = std::numeric_limits<float>::lowest();

		Vector3f center = Vector3f::Zero;
		for (UIndex64 v = firstVertexId; v < vertices.GetSize(); v++)
			center += vertices[v].position;
		center /= static_cast<float>(vertices.GetSize() - firstVertexId);


		for (UIndex64 v = firstVertexId; v < vertices.GetSize(); v++) {
			const float distance2 = vertices[v].position.GetDistanceTo2(center);
			radius = glm::max(radius, distance2);
		}
		radius = glm::sqrt(radius);

		auto mesh = Mesh3D(
			static_cast<USize32>(primitiveIndices.GetSize()),
			firstIndexId,
			center,
			m_meshes.GetSize());

		mesh.SetBoundingSphereRadius(radius);

		m_meshes.Insert(mesh);
	}

	for (UIndex32 i = 0; i < node.children.size(); i++)
		ProcessNode(model, model.nodes[node.children[i]], node.children[i], parentId);
}

void StaticMeshLoader::SetupModel(Model3D* model) {
	model->_SetVertexBuffer(Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(vertices, Vertex3D::GetVertexInfo()));
	
	IGltfLoader::SetupModel(model);
}
