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

	// Proceso del pol�gono.
	if (node.mesh > -1) {
		const tinygltf::Mesh& mesh = gltfModel.meshes[node.mesh];

		for (TSize i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& primitive = mesh.primitives[i];

			if (primitive.material > -1)
				meshIdToMaterialId.Insert(meshes.GetSize(), primitive.material);

			OSK_ASSERT(primitive.mode == TINYGLTF_MODE_TRIANGLES, "El modelo no est� en modo TRI�NGULOS.");

			TSize numVertices = 0;
			const TSize firstVertexId = vertices.GetSize();
			const TSize firstIndexId = indices.GetSize();

			// Los datos se almacenan en forma de buffers.
			const float* positionsBuffer = nullptr;
			const float* normalsBuffer = nullptr;
			const float* texCoordsBuffer = nullptr;
			const void* colorBuffer = nullptr;

			int colorDataType = 0;
			int colorVectorType = 0;

			// Comprobamos que tiene almacenado info de posici�n.
			if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
				// Para poder acceder a la informaci�n en forma de buffer.
				const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				positionsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				numVertices = static_cast<TSize>(accessor.count);
			}

			// Comprobamos que tiene almacenado info de normales.
			if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
				// Para poder acceder a la informaci�n en forma de buffer.
				const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("NORMAL")->second];
				const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				normalsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			// Comprobamos que tiene almacenado info de coordenadas de texturas.
			if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
				// Para poder acceder a la informaci�n en forma de buffer.
				const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("TEXCOORD_0")->second];
				const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				texCoordsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			// Comprobamos que tiene almacenado el color del v�rtice.
			if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
				// Para poder acceder a la informaci�n en forma de buffer.
				const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("COLOR_0")->second];
				const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				colorBuffer = &(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);

				colorVectorType = accessor.type;
				colorDataType = accessor.componentType;
			}

			OSK_ASSERT(positionsBuffer != nullptr, "No se encontraron posiciones.");
			OSK_ASSERT(normalsBuffer != nullptr, "No se encontraron normales.");
			OSK_ASSERT(texCoordsBuffer != nullptr, "No se encontraron coordenadas de texturas.");

			// Procesamos los buffers y generamos nuevos v�rtices.
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

				vertex.color = Color::WHITE();
				if (primitive.material > -1)
					vertex.color = modelInfo.materialInfos[primitive.material].baseColor;
				
				if (colorBuffer) {
					const int stride = colorVectorType;

					switch (colorDataType) {

					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
						constexpr float denom = std::numeric_limits<uint8_t>::max();
						vertex.color += {
							static_cast<float>(static_cast<const uint8_t*>(colorBuffer)[v * stride + 0] / denom),
							static_cast<float>(static_cast<const uint8_t*>(colorBuffer)[v * stride + 1] / denom),
							static_cast<float>(static_cast<const uint8_t*>(colorBuffer)[v * stride + 2] / denom),
							1.0f
						};

						break;
					}

					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
						constexpr float denom = std::numeric_limits<uint16_t>::max();
						vertex.color += {
							static_cast<float>(static_cast<const uint16_t*>(colorBuffer)[v * stride + 0] / denom),
							static_cast<float>(static_cast<const uint16_t*>(colorBuffer)[v * stride + 1] / denom),
							static_cast<float>(static_cast<const uint16_t*>(colorBuffer)[v * stride + 2] / denom),
							1.0f
						};

						break;
					}

					case TINYGLTF_PARAMETER_TYPE_FLOAT: {
						vertex.color += Color{
							static_cast<float>(static_cast<const float*>(colorBuffer)[v * stride + 0]),
							static_cast<float>(static_cast<const float*>(colorBuffer)[v * stride + 1]),
							static_cast<float>(static_cast<const float*>(colorBuffer)[v * stride + 2]),
							1.0f
						};

						break;
					}

					}
				}
				

				vertices.Insert(vertex);
			}

			// �ndices
			const tinygltf::Accessor& indicesAccesor = gltfModel.accessors[primitive.indices];
			const tinygltf::BufferView& indicesView = gltfModel.bufferViews[indicesAccesor.bufferView];
			const tinygltf::Buffer& indicesBuffer = gltfModel.buffers[indicesView.buffer];

			// Los �ndices tambien se guardan en buffers.
			// Necesitamos saber su tipo para procesar el buffer.
			// El �ndice es el ID del v�rtice dentro de la primitiva.
			// Para obtener el ID real, debemos tener en cuenta todos los v�rtices anteriormente procesados.
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