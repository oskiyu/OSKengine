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
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "IGpuImage.h"
#include "GpuImageLayout.h"
#include "OwnedPtr.h"
#include "GpuImageUsage.h"
#include "GpuImageDimensions.h"
#include "Color.hpp"
#include "Vertex3D.h"

#include <gtc/type_ptr.hpp>
#include <json.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

// STRUCTS

/// <summary>
/// Información relevante de un material del modelo GLTF.
/// </summary>
struct GltfMaterialInfo {

	/// <summary>
	/// Color base.
	/// Para los vértices.
	/// </summary>
	Color baseColor;

	/// <summary>
	/// ID de la textura base.
	/// </summary>
	TSize baseTextureIndex = 0;

	/// <summary>
	/// True si tiene textura.
	/// </summary>
	bool hasBaseTexture = false;

};

/// <summary>
/// Información relevante de un modelo GLTF,
/// necesario para la correcta generación de meshes.
/// </summary>
struct GltfModelInfo {

	/// <summary>
	/// Información sobre todos los materiales del modelo.
	/// ID del material = posición dentro del array.
	/// </summary>
	DynamicArray<GltfMaterialInfo> materialInfos;

};

glm::mat4 GetNodeMatrix(const tinygltf::Node& node) {
	// Obtenemos el transform del nodo.
	// Puede estar definido de varias maneras:

	glm::mat4 nodeMatrix = glm::mat4(1.0f);

	// Definido por una posición.
	if (node.translation.size() == 3) {
		nodeMatrix = glm::translate(nodeMatrix, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
	}
	// Definido por una rotación.
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
/// Procesa un nodo del modelo, además de
/// todos sus nodos hijos.
/// Genera los meshes, vértices e índices.
/// </summary>
/// <param name="node">Nodo del modelo GLTF.</param>
/// <param name="model">Modelo GLTF original.</param>
/// <param name="modelInfo">Información relevante del modelo para la generación de meshes.</param>
/// <param name="meshes">Lista donde se introducirán los meshes.</param>
/// <param name="vertices">Lista donde se introducirán los vértices.</param>
/// <param name="indices">Lista donde se introducirán los índices.</param>
/// <param name="prevMatrix">Matriz offset del nodo padre.</param>
/// <param name="globalScale">Escala del modelo.</param>
void ProcessMeshNode(const tinygltf::Node& node, const tinygltf::Model& model, const GltfModelInfo& modelInfo, HashMap<TSize, TSize>* meshIdToMaterialId, DynamicArray<Mesh3D>* meshes, DynamicArray<Vertex3D>* vertices, DynamicArray<TIndexSize>* indices, const glm::mat4& prevMatrix, float globalScale) {
	glm::mat4 nodeMatrix = prevMatrix * GetNodeMatrix(node);

	// Proceso del polígono.
	if (node.mesh > -1) {
		const tinygltf::Mesh& mesh = model.meshes[node.mesh];
		
		if (mesh.primitives[0].material > -1)
			meshIdToMaterialId->Insert(meshes->GetSize(), mesh.primitives[0].material);
		
		for (TSize i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& primitive = mesh.primitives[i];
			TSize numVertices = 0;
			TSize firstVertexId = vertices->GetSize();
			TSize firstIndexId = indices->GetSize();

			// Los datos se almacenan en forma de buffers.
			const float* positionsBuffer = nullptr;
			const float* normalsBuffer = nullptr;
			const float* texCoordsBuffer = nullptr;

			// Comprobamos que tiene almacenado info de posición.
			if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
				// Para poder acceder a la información en forma de buffer.
				const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				positionsBuffer = (const float*)(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				numVertices = accessor.count;
			}

			// Comprobamos que tiene almacenado info de normales.
			if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
				// Para poder acceder a la información en forma de buffer.
				const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("NORMAL")->second];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				normalsBuffer = (const float*)(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			// Comprobamos que tiene almacenado info de coordenadas de texturas.
			if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
				// Para poder acceder a la información en forma de buffer.
				const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

				// Leemos el buffer.
				texCoordsBuffer = (const float*)(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			// Procesamos los buffers y generamos nuevos vértices.
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

				if (primitive.material > -1)
					vertex.color = modelInfo.materialInfos[primitive.material].baseColor;
				else
					vertex.color = 1.0f;
				

				vertices->Insert(vertex);
			}

			// Índices
			const tinygltf::Accessor& indicesAccesor = model.accessors[primitive.indices];
			const tinygltf::BufferView& indicesView = model.bufferViews[indicesAccesor.bufferView];
			const tinygltf::Buffer& indicesBuffer = model.buffers[indicesView.buffer];

			// Los índices tambien se guardan en buffers.
			// Necesitamos saber su tipo para procesar el buffer.
			// El índice es el ID del vértice dentro de la primitiva.
			// Para obtener el ID real, debemos tener en cuenta todos los vértices anteriormente procesados.
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
		ProcessMeshNode(model.nodes[node.children[i]], model, modelInfo, meshIdToMaterialId, meshes, vertices, indices, nodeMatrix, globalScale);
}

/// <summary>
/// Carga todos los materiales del modelo,
/// y guarda la información relevante de cada uno.
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

		if (model.materials[i].pbrMetallicRoughness.baseColorTexture.index != -1) {
			info.hasBaseTexture = true;
			info.baseTextureIndex = model.textures[model.materials[i].pbrMetallicRoughness.baseColorTexture.index].source;
		}
		
		output[i] = info;
	}

	return output;
}

DynamicArray<OwnedPtr<GRAPHICS::GpuImage>> LoadImages(const tinygltf::Model& model) {
	DynamicArray<OwnedPtr<GRAPHICS::GpuImage>> output;

	for (TSize i = 0; i < model.images.size(); i++) {
		const tinygltf::Image& originalImage = model.images[i];
		
		auto image = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage({ (unsigned int)originalImage.width, (unsigned int)originalImage.height, 1 },
			GRAPHICS::GpuImageDimension::d2D, 1, GRAPHICS::Format::RGBA8_UNORM, GpuImageUsage::TRANSFER_DESTINATION | GRAPHICS::GpuImageUsage::SAMPLED, 
			GRAPHICS::GpuSharedMemoryType::GPU_ONLY, true);

		const TSize numBytes = originalImage.width * originalImage.height * 4;
		if (originalImage.component == 3) {
			TByte* data = (TByte*)malloc(numBytes);
			memset(data, 255, numBytes);

			TSize rgbPos = 0;
			for (TSize i = 0; i < numBytes; i += 4) {
				memcpy(data + i, originalImage.image.data() + rgbPos, 3);
				rgbPos += 3;
			}

			Engine::GetRenderer()->UploadImageToGpu(image.GetPointer(), originalImage.image.data(), numBytes, GRAPHICS::GpuImageLayout::SHADER_READ_ONLY);

			free(data);
		}

		else if (originalImage.component == 4) {
			Engine::GetRenderer()->UploadImageToGpu(image.GetPointer(), originalImage.image.data(), numBytes, GRAPHICS::GpuImageLayout::SHADER_READ_ONLY);
		}

		output.Insert(image);
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

	DynamicArray<Vertex3D> vertices;
	DynamicArray<TIndexSize> indices;
	DynamicArray<Mesh3D> meshes;
	DynamicArray<OwnedPtr<GpuImage>> textures = LoadImages(gltfModel);
	HashMap<TSize, TSize> meshIdToMaterialId;

	tinygltf::Scene scene = gltfModel.scenes[0];
	
	GltfModelInfo modelInfo{};
	modelInfo.materialInfos = LoadMaterials(gltfModel);

	for (TSize i = 0; i < scene.nodes.size(); i++)
		ProcessMeshNode(gltfModel.nodes[scene.nodes[i]], gltfModel, modelInfo, &meshIdToMaterialId, &meshes, &vertices, &indices, glm::mat4(1.0f), assetInfo["scale"]);

	// GPU.
	output->_SetVertexBuffer(Engine::GetRenderer()->GetMemoryAllocator()->CreateVertexBuffer(vertices));
	output->_SetIndexBuffer(Engine::GetRenderer()->GetMemoryAllocator()->CreateIndexBuffer(indices));

	output->_SetIndexCount(indices.GetSize());

	for (TSize i = 0; i < meshes.GetSize(); i++) {
		MeshMetadata meshMetadata{};

		if (meshIdToMaterialId.ContainsKey(i)) {
			GltfMaterialInfo& materialInfo = modelInfo.materialInfos.At(meshIdToMaterialId.Get(i));
			if (materialInfo.hasBaseTexture)
				meshMetadata.materialTextures.Insert("baseTexture", materialInfo.baseTextureIndex);
		}

		output->AddMesh(meshes.At(i), meshMetadata);
	}

	for (TSize i = 0; i < textures.GetSize(); i++)
		output->AddGpuImage(textures.At(i));
}
