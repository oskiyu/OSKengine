#include "StaticMeshLoader.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"

#include <ext/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

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

		if (mesh.primitives[0].material > -1)
			meshIdToMaterialId.Insert(meshes.GetSize(), mesh.primitives[0].material);

		for (TSize i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& primitive = mesh.primitives[i];

			OSK_ASSERT(primitive.mode == TINYGLTF_MODE_TRIANGLES, "El modelo no está en modo TRIÁNGULOS.");

			TSize numVertices = 0;
			const TSize firstVertexId = vertices.GetSize();
			const TSize firstIndexId = indices.GetSize();

			// Los datos se almacenan en forma de buffers.
			const float* positionsBuffer = nullptr;
			const float* normalsBuffer = nullptr;
			const float* texCoordsBuffer = nullptr;

			// Comprobamos que tiene almacenado info de posición.
			if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
				// Para poder acceder a la información en forma de buffer.
				const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				positionsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				numVertices = static_cast<TSize>(accessor.count);
			}

			// Comprobamos que tiene almacenado info de normales.
			if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
				// Para poder acceder a la información en forma de buffer.
				const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("NORMAL")->second];
				const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				normalsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			// Comprobamos que tiene almacenado info de coordenadas de texturas.
			if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
				// Para poder acceder a la información en forma de buffer.
				const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("TEXCOORD_0")->second];
				const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				texCoordsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			OSK_ASSERT(positionsBuffer != nullptr, "No se encontraron posiciones.");
			OSK_ASSERT(normalsBuffer != nullptr, "No se encontraron normales.");
			OSK_ASSERT(texCoordsBuffer != nullptr, "No se encontraron coordenadas de texturas.");

			// Procesamos los buffers y generamos nuevos vértices.
			for (TSize v = 0; v < numVertices; v++) {
				glm::vec4 vertexPosition = glm::vec4(
					positionsBuffer[v * 3 + 0],
					positionsBuffer[v * 3 + 1],
					positionsBuffer[v * 3 + 2],
					globalScale
				);

				Vertex3D vertex{};
				vertex.position = glm::vec3(nodeMatrix * vertexPosition);

				const Vector3f originalNormal = Vector3f{
					normalsBuffer[v * 3 + 0],
					normalsBuffer[v * 3 + 1],
					normalsBuffer[v * 3 + 2]
				}.GetNormalized();

				// Las normales se calculan aparte.

				vertex.texCoords = {
					texCoordsBuffer[v * 2 + 0],
					texCoordsBuffer[v * 2 + 1]
				};

				if (primitive.material > -1)
					vertex.color = modelInfo.materialInfos[primitive.material].baseColor;
				else
					vertex.color = 1.0f;

				vertices.Insert(vertex);
			}

			// Índices
			const tinygltf::Accessor& indicesAccesor = gltfModel.accessors[primitive.indices];
			const tinygltf::BufferView& indicesView = gltfModel.bufferViews[indicesAccesor.bufferView];
			const tinygltf::Buffer& indicesBuffer = gltfModel.buffers[indicesView.buffer];

			// Los índices tambien se guardan en buffers.
			// Necesitamos saber su tipo para procesar el buffer.
			// El índice es el ID del vértice dentro de la primitiva.
			// Para obtener el ID real, debemos tener en cuenta todos los vértices anteriormente procesados.
			switch (indicesAccesor.componentType) {
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
				const uint32_t* indBuffer = (const uint32_t*)(&indicesBuffer.data[indicesAccesor.byteOffset + indicesView.byteOffset]);
				for (TSize index = 0; index < indicesAccesor.count; index++)
					indices.Insert((TIndexSize)indBuffer[index] + firstVertexId);

				break;
			}

			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
				const uint16_t* indBuffer = (const uint16_t*)(&indicesBuffer.data[indicesAccesor.byteOffset + indicesView.byteOffset]);
				for (size_t index = 0; index < indicesAccesor.count; index++)
					indices.Insert((TIndexSize)indBuffer[index] + firstVertexId);

				break;
			}

			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
				const uint8_t* indBuffer = (const uint8_t*)(&indicesBuffer.data[indicesAccesor.byteOffset + indicesView.byteOffset]);
				for (size_t index = 0; index < indicesAccesor.count; index++)
					indices.Insert((TIndexSize)indBuffer[index] + firstVertexId);

				break;
			}

			}

			meshes.Insert({ (TSize)indicesAccesor.count, firstIndexId });
		}
	}

	for (TSize i = 0; i < node.children.size(); i++)
		ProcessNode(gltfModel.nodes[node.children[i]], node.children[i], parentId);
}

void StaticMeshLoader::SetupModel(Model3D* model) {
	model->_SetVertexBuffer(Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(vertices, Vertex3D::GetVertexInfo()));
	
	IMeshLoader::SetupModel(model);
}
