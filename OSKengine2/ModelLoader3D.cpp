#include "ModelLoader3D.h"

#include "Model3D.h"
#include "DynamicArray.hpp"
#include "Vertex.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuVertexBuffer.h"
#include "IGpuIndexBuffer.h"
#include "IGpuMemoryAllocator.h"
#include "FileIO.h"
#include "Mesh3D.h"

#include <gtc/type_ptr.hpp>
#include <json.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

using namespace OSK;
using namespace OSK::ASSETS;

// STRUCTS

/// <summary>
/// Informaci�n relevante de un material del modelo GLTF.
/// </summary>
struct GltfMaterialInfo {

	/// <summary>
	/// Color base.
	/// Para los v�rtices.
	/// </summary>
	Color baseColor;

};

/// <summary>
/// Informaci�n relevante de un modelo GLTF,
/// necesario para la correcta generaci�n de meshes.
/// </summary>
struct GltfModelInfo {

	/// <summary>
	/// Informaci�n sobre todos los materiales del modelo.
	/// ID del material = posici�n dentro del array.
	/// </summary>
	DynamicArray<GltfMaterialInfo> materialInfos;

};

glm::mat4 GetNodeMatrix(const tinygltf::Node& node) {
	// Obtenemos el transform del nodo.
	// Puede estar definido de varias maneras:

	glm::mat4 nodeMatrix = glm::mat4(1.0f);

	// Definido por una posici�n.
	if (node.translation.size() == 3) {
		nodeMatrix = glm::translate(nodeMatrix, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
	}
	// Definido por una rotaci�n.
	if (node.rotation.size() == 4) {
		glm::quat quaternion = glm::make_quat(node.rotation.data());
		nodeMatrix *= glm::mat4(quaternion);
	}
	// Definido por una escala.
	if (node.scale.size() == 3) {
		nodeMatrix = glm::scale(nodeMatrix, glm::vec3(glm::make_vec3(node.scale.data())));
	}
	// Definido por una matriz.
	if (node.matrix.size() == 16) {
		nodeMatrix = glm::make_mat4x4(node.matrix.data());
	}

	return nodeMatrix;
}

/// <summary>
/// Procesa un nodo del modelo, adem�s de
/// todos sus nodos hijos.
/// Genera los meshes, v�rtices e �ndices.
/// </summary>
/// <param name="node">Nodo del modelo GLTF.</param>
/// <param name="model">Modelo GLTF original.</param>
/// <param name="modelInfo">Informaci�n relevante del modelo para la generaci�n de meshes.</param>
/// <param name="meshes">Lista donde se introducir�n los meshes.</param>
/// <param name="vertices">Lista donde se introducir�n los v�rtices.</param>
/// <param name="indices">Lista donde se introducir�n los �ndices.</param>
/// <param name="prevMatrix">Matriz offset del nodo padre.</param>
/// <param name="globalScale">Escala del modelo.</param>
void ProcessMeshNode(const tinygltf::Node& node, const tinygltf::Model& model, const GltfModelInfo& modelInfo, DynamicArray<GRAPHICS::Mesh3D>* meshes, DynamicArray<GRAPHICS::Vertex3D>* vertices, DynamicArray<GRAPHICS::TIndexSize>* indices, const glm::mat4& prevMatrix, float globalScale) {
	glm::mat4 nodeMatrix = prevMatrix * GetNodeMatrix(node);

	// Proceso del pol�gono.
	if (node.mesh > -1) {
		const tinygltf::Mesh& mesh = model.meshes[node.mesh];
		
		for (TSize i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& primitive = mesh.primitives[i];
			TSize numVertices = 0;
			TSize firstVertexId = vertices->GetSize();
			TSize firstIndexId = indices->GetSize();
			
			// Los datos se almacenan en forma de buffers.
			const float* positionsBuffer = nullptr;
			const float* normalsBuffer = nullptr;
			const float* texCoordsBuffer = nullptr;

			// Comprobamos que tiene almacenado info de posici�n.
			if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
				// Para poder acceder a la informaci�n en forma de buffer.
				const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				positionsBuffer = (const float*)(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				numVertices = accessor.count;
			}

			// Comprobamos que tiene almacenado info de normales.
			if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
				// Para poder acceder a la informaci�n en forma de buffer.
				const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("NORMAL")->second];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				normalsBuffer = (const float*)(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			// Comprobamos que tiene almacenado info de coordenadas de texturas.
			if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
				// Para poder acceder a la informaci�n en forma de buffer.
				const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				texCoordsBuffer = (const float*)(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			// Procesamos los buffers y generamos nuevos v�rtices.
			for (TSize v = 0; v < numVertices; v++) {
				glm::vec4 vertexPosition = glm::vec4(
					positionsBuffer[v * 3],
					positionsBuffer[v * 3 + 1],
					positionsBuffer[v * 3 + 2],
					globalScale
				);

				GRAPHICS::Vertex3D vertex{};
				vertex.position = glm::vec3(glm::scale(nodeMatrix, glm::vec3(globalScale)) * vertexPosition);
				vertex.normal = glm::normalize(glm::vec3(nodeMatrix * glm::vec4(
					normalsBuffer[v * 3],
					normalsBuffer[v * 3 + 1],
					normalsBuffer[v * 3 + 2],
					globalScale
				)));
				vertex.texCoords = {
					texCoordsBuffer[v * 2],
					texCoordsBuffer[v * 2 + 1]
				};

				if (primitive.material > 0)
					vertex.color = modelInfo.materialInfos[primitive.material].baseColor;
				else
					vertex.color = 1.0f;
				

				vertices->Insert(vertex);
			}

			// �ndices
			const tinygltf::Accessor& indicesAccesor = model.accessors[primitive.indices];
			const tinygltf::BufferView& indicesView = model.bufferViews[indicesAccesor.bufferView];
			const tinygltf::Buffer& indicesBuffer = model.buffers[indicesView.buffer];

			// Los �ndices tambien se guardan en buffers.
			// Necesitamos saber su tipo para procesar el buffer.
			// El �ndice es el ID del v�rtice dentro de la primitiva.
			// Para obtener el ID real, debemos tener en cuenta todos los v�rtices anteriormente procesados.
			switch (indicesAccesor.componentType) {
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
					const uint32_t* indBuffer = (const uint32_t*)(&indicesBuffer.data[indicesAccesor.byteOffset + indicesView.byteOffset]);
					for (TSize index = 0; index < indicesAccesor.count; index++)
						indices->Insert((GRAPHICS::TIndexSize)indBuffer[index] + firstVertexId);

					break;
				}

				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
					const uint16_t* indBuffer = (const uint16_t*)(&indicesBuffer.data[indicesAccesor.byteOffset + indicesView.byteOffset]);
					for (size_t index = 0; index < indicesAccesor.count; index++)
						indices->Insert((GRAPHICS::TIndexSize)indBuffer[index] + firstVertexId);

					break;
				}

				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
					const uint8_t* indBuffer = (const uint8_t*)(&indicesBuffer.data[indicesAccesor.byteOffset + indicesView.byteOffset]);
					for (size_t index = 0; index < indicesAccesor.count; index++)
						indices->Insert((GRAPHICS::TIndexSize)indBuffer[index] + firstVertexId);

					break;
				}

			}

			meshes->Insert({ (TSize)indicesAccesor.count, firstIndexId });
		}
	}
	for (TSize i = 0; i < node.children.size(); i++)
		ProcessMeshNode(model.nodes[node.children[i]], model, modelInfo, meshes, vertices, indices, nodeMatrix, globalScale);
}

/// <summary>
/// Carga todos los materiales del modelo,
/// y guarda la informaci�n relevante de cada uno.
/// </summary>
DynamicArray<GltfMaterialInfo> LoadMaterials(const tinygltf::Model& model) {
	auto output = DynamicArray<GltfMaterialInfo>::CreateResizedArray(model.materials.size());

	for (TSize i = 0; i < model.materials.size(); i++) {
		GltfMaterialInfo info{};
		info.baseColor = {
			(float)model.materials[i].pbrMetallicRoughness.baseColorFactor[0],
			(float)model.materials[i].pbrMetallicRoughness.baseColorFactor[1],
			(float)model.materials[i].pbrMetallicRoughness.baseColorFactor[2],
			(float)model.materials[i].pbrMetallicRoughness.baseColorFactor[3],
		};
		
		output[i] = info;
	}

	return output;
}

void ModelLoader3D::Load(const std::string& assetFilePath, IAsset** asset) {
	Model3D* output = (Model3D*)*asset;

	// a
	nlohmann::json assetInfo = nlohmann::json::parse(IO::FileIO::ReadFromFile(assetFilePath));
	std::string texturePath = assetInfo["raw_asset_path"];
	output->SetName(assetInfo["name"]);

	OSK_ASSERT(IO::FileIO::FileExists(assetInfo["raw_asset_path"]), "El modelo en no existe.");

	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF context;

	std::string errorMessage;
	std::string warningMessage;

	context.LoadBinaryFromFile(&gltfModel, &errorMessage, &warningMessage, assetInfo["raw_asset_path"]);

	DynamicArray<GRAPHICS::Vertex3D> vertices;
	DynamicArray<GRAPHICS::TIndexSize> indices;
	DynamicArray<GRAPHICS::Mesh3D> meshes;

	tinygltf::Scene scene = gltfModel.scenes[0];
	
	GltfModelInfo modelInfo{};
	modelInfo.materialInfos = LoadMaterials(gltfModel);
	for (TSize i = 0; i < scene.nodes.size(); i++)
		ProcessMeshNode(gltfModel.nodes[scene.nodes[i]], gltfModel, modelInfo, &meshes, &vertices, &indices, glm::mat4(1.0f), assetInfo["scale"]);

	// GPU.
	output->_SetVertexBuffer(Engine::GetRenderer()->GetMemoryAllocator()->CreateVertexBuffer(vertices));
	output->_SetIndexBuffer(Engine::GetRenderer()->GetMemoryAllocator()->CreateIndexBuffer(indices));

	output->_SetIndexCount(vertices.GetSize());

	for (TSize i = 0; i < meshes.GetSize(); i++)
		output->AddMesh(meshes.At(i));
}
