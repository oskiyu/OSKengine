#include "StaticMeshLoader.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void StaticMeshLoader::SmoothNormals() {
	for (TSize i = 0; i < indices.GetSize(); i += 3) {
		const TIndexSize localIndices[3] = {
			indices.At(i + 0),
			indices.At(i + 1),
			indices.At(i + 2)
		};

		const Vertex3D localVertices[3] = {
			vertices.At(localIndices[0]),
			vertices.At(localIndices[1]),
			vertices.At(localIndices[2])
		};

		const Vector3f v0 = localVertices[1].position - localVertices[0].position;
		const Vector3f v1 = localVertices[2].position - localVertices[0].position;

		const Vector3f faceNormal = v0.Cross(v1).GetNormalized();

		for (TSize li = 0; li < 3; li++)
			vertices.At(localIndices[li]).normal += faceNormal;
	}

	for (TSize v = 0; v < vertices.GetSize(); v++)
		vertices.At(v).normal.Normalize();
}

void StaticMeshLoader::ProcessNode(const tinygltf::Node& node, TSize nodeId, TSize parentId) {
	const glm::mat4 nodeMatrix = modelTransform * GetNodeMatrix(node);

	// Proceso del polígono.
	if (node.mesh > -1) {
		const tinygltf::Mesh& mesh = gltfModel.meshes[node.mesh];

		for (TSize i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& primitive = mesh.primitives[i];

			if (primitive.material > -1)
				meshIdToMaterialId.Insert(meshes.GetSize(), primitive.material);

			OSK_ASSERT(primitive.mode == TINYGLTF_MODE_TRIANGLES, "El modelo no está en modo TRIÁNGULOS.");

			const TSize firstVertexId = vertices.GetSize();
			const TSize firstIndexId = indices.GetSize();

			const auto primitiveIndices = GetIndices(primitive, firstVertexId);

			const auto positions = GetVertexPositions(primitive, nodeMatrix);
			const auto normals = GetVertexNormals(primitive);
			const auto texCoords = GetTextureCoords(primitive);
			const auto colors = GetVertexColors(primitive);

			const auto tangents = HasTangets(primitive)
				? GetTangentVectors(primitive)
				: GenerateTangetVectors(texCoords, positions, primitiveIndices, firstVertexId);

			const TSize numVertices = positions.GetSize();

			const bool hasColors = !colors.IsEmpty();

			// Procesamos los buffers y generamos nuevos vértices.
			for (TSize v = 0; v < numVertices; v++) {

				Vertex3D vertex{};
				vertex.position = positions[v];
				vertex.normal = normals[v];
				vertex.texCoords = texCoords[v];
				vertex.color = Color::WHITE();

				if (HasTangets(primitive))
					vertex.tangent = tangents[v];

				if (primitive.material > -1)
					vertex.color = modelInfo.materialInfos[primitive.material].baseColor;
				
				if (hasColors)
					vertex.color = colors[v];
				
				vertices.Insert(vertex);
			}

			indices.InsertAll(primitiveIndices);

			meshes.Insert(Mesh3D(primitiveIndices.GetSize(), firstIndexId));
		}
	}

	for (TSize i = 0; i < node.children.size(); i++)
		ProcessNode(gltfModel.nodes[node.children[i]], node.children[i], parentId);
}

void StaticMeshLoader::SetupModel(Model3D* model) {
	model->_SetVertexBuffer(Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(vertices, Vertex3D::GetVertexInfo()));
	
	IMeshLoader::SetupModel(model);
}
