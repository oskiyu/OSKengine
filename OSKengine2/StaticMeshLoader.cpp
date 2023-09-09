#include "StaticMeshLoader.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"

#include "ModelLoadingExceptions.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void StaticMeshLoader::SmoothNormals() {
	for (UIndex32 i = 0; i < m_indices.GetSize(); i += 3) {
		const TIndexSize localIndices[3] = {
			m_indices.At(i + 0),
			m_indices.At(i + 1),
			m_indices.At(i + 2)
		};

		const Vertex3D localVertices[3] = {
			vertices.At(localIndices[0]),
			vertices.At(localIndices[1]),
			vertices.At(localIndices[2])
		};

		const Vector3f v0 = localVertices[1].position - localVertices[0].position;
		const Vector3f v1 = localVertices[2].position - localVertices[0].position;

		const Vector3f faceNormal = v0.Cross(v1).GetNormalized();

		for (UIndex32 li = 0; li < 3; li++)
			vertices.At(localIndices[li]).normal += faceNormal;
	}

	for (UIndex32 v = 0; v < vertices.GetSize(); v++)
		vertices.At(v).normal.Normalize();
}

void StaticMeshLoader::ProcessNode(const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) {
	const glm::mat4 nodeMatrix = m_modelTransform * GetNodeMatrix(node);
	const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(nodeMatrix)));

	// Proceso del polígono.
	if (node.mesh > -1) {
		const tinygltf::Mesh& mesh = m_gltfModel.meshes[node.mesh];

		for (UIndex32 i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& primitive = mesh.primitives[i];

			if (primitive.material > -1)
				m_meshIdToMaterialId[static_cast<UIndex32>(m_meshes.GetSize())] = primitive.material;

			OSK_ASSERT(primitive.mode == TINYGLTF_MODE_TRIANGLES, UnsupportedPolygonModeException(std::to_string(primitive.mode)));

			const UIndex64 firstVertexId = vertices.GetSize();
			const UIndex64 firstIndexId = m_indices.GetSize();

			const auto primitiveIndices = GetIndices(primitive, firstVertexId);

			const auto positions = GetVertexPositions(primitive, nodeMatrix);
			const auto normals = GetVertexNormals(primitive);
			const auto texCoords = GetTextureCoords(primitive);
			const auto colors = GetVertexColors(primitive);

			const auto tangents = HasTangets(primitive)
				? GetTangentVectors(primitive)
				: GenerateTangetVectors(texCoords, positions, primitiveIndices, firstVertexId);

			const auto numVertices = positions.GetSize();

			const bool hasColors = !colors.IsEmpty();

			// Procesamos los buffers y generamos nuevos vértices.
			for (UIndex64 v = 0; v < numVertices; v++) {

				Vertex3D vertex{};
				vertex.position = positions[v];
				vertex.normal = glm::normalize(normalMatrix * normals[v].ToGLM());
				vertex.texCoords = texCoords[v];
				vertex.color = Color::White;

				vertex.tangent = glm::normalize(normalMatrix * tangents[v].ToGLM());

				if (primitive.material > -1)
					vertex.color = m_modelInfo.materialInfos[primitive.material].baseColor;
				
				if (hasColors)
					vertex.color = colors[v];
				
				vertices.Insert(vertex);
			}

			m_indices.InsertAll(primitiveIndices);

			m_meshes.Insert(Mesh3D(static_cast<USize32>(primitiveIndices.GetSize()), firstIndexId));
		}
	}

	for (UIndex32 i = 0; i < node.children.size(); i++)
		ProcessNode(m_gltfModel.nodes[node.children[i]], node.children[i], parentId);
}

void StaticMeshLoader::SetupModel(Model3D* model) {
	model->_SetVertexBuffer(Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(vertices, Vertex3D::GetVertexInfo()));
	
	IMeshLoader::SetupModel(model);
}
