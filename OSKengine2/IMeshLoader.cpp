#include "IMeshLoader.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"

#include "ModelLoadingExceptions.h"
#include "BadAllocException.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void IMeshLoader::Load(const std::string& rawAssetPath, const glm::mat4& modelTransform) {
	this->modelTransform = modelTransform;

	tinygltf::TinyGLTF context;

	std::string errorMessage;
	std::string warningMessage;

	context.LoadBinaryFromFile(&gltfModel, &errorMessage, &warningMessage, rawAssetPath);
	OSK_ASSERT(errorMessage.empty(), EngineException("Error al cargar modelo estático: " + errorMessage));

	modelInfo.materialInfos = LoadMaterials();
	const tinygltf::Scene scene = gltfModel.scenes[0];

	for (const int nodeIndex : scene.nodes) {
		const tinygltf::Node& node = gltfModel.nodes[nodeIndex];
		constexpr auto noParentId = std::numeric_limits<UIndex32>::max();

		ProcessNode(node, nodeIndex, noParentId);
	}

	// SmoothNormals();
}

void IMeshLoader::SetupModel(Model3D* model) {
	model->_SetIndexBuffer(Engine::GetRenderer()->GetAllocator()->CreateIndexBuffer(indices));
	model->_SetIndexCount(indices.GetSize());

	for (UIndex32 i = 0; i < meshes.GetSize(); i++) {
		MeshMetadata meshMetadata{};

		if (meshIdToMaterialId.ContainsKey(i)) {
			const auto& materialInfo = modelInfo.materialInfos.At(meshIdToMaterialId.Get(i));

			if (materialInfo.hasBaseTexture)
				meshMetadata.materialTextures.Insert("baseTexture", materialInfo.baseTextureIndex);

			if (materialInfo.hasNormalTexture)
				meshMetadata.materialTextures.Insert("normalTexture", materialInfo.normalTextureIndex);

			meshMetadata.metallicFactor = materialInfo.metallicFactor;
			meshMetadata.roughnessFactor = materialInfo.roughnessFactor;
		}

		model->AddMesh(meshes.At(i), meshMetadata);
	}

	if (Engine::GetRenderer()->IsRtActive())
		model->_SetAccelerationStructure(Engine::GetRenderer()->GetAllocator()->CreateBottomAccelerationStructure(*model->GetVertexBuffer(), *model->GetIndexBuffer()));

	DynamicArray<OwnedPtr<GpuImage>> textures = LoadImages();
	for (UIndex32 i = 0; i < textures.GetSize(); i++)
		model->AddGpuImage(textures.At(i));
}

glm::mat4 IMeshLoader::GetNodeMatrix(const tinygltf::Node& node) {
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

DynamicArray<OwnedPtr<GpuImage>> IMeshLoader::LoadImages() {
	DynamicArray<OwnedPtr<GpuImage>> output;

	/// @todo Single upload cmd list.

	for (UIndex32 i = 0; i < gltfModel.images.size(); i++) {
		const tinygltf::Image& originalImage = gltfModel.images[i];

		OwnedPtr<GpuImage> image = Engine::GetRenderer()->GetAllocator()->CreateImage(
			GpuImageCreateInfo::CreateDefault2D({ 
				(unsigned int)originalImage.width, 
				(unsigned int)originalImage.height },
			Format::RGBA8_UNORM,
			GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED)
		);
		image->SetDebugName(std::format("Mesh texture {}", i));

		OwnedPtr<ICommandList> uploadCmdList = Engine::GetRenderer()->CreateSingleUseCommandList();
		uploadCmdList->Reset();
		uploadCmdList->Start();

		const USize64 numBytes = originalImage.width * originalImage.height * 4;
		if (originalImage.component == 3) {
			TByte* data = new TByte[numBytes];

			OSK_ASSERT(data != NULL, BadAllocException());
			OSK_ASSUME(data != nullptr);

			memset(data, 255, numBytes);

			UIndex64 rgbPos = 0;
			for (UIndex64 i = 0; i < numBytes; i += 4) {
				OSK_ASSUME(i < numBytes);
				memcpy(&data[i], originalImage.image.data() + rgbPos, 3);
				rgbPos += 3;
			}

			uploadCmdList->SetGpuImageBarrier(
				image.GetPointer(), 
				GpuImageLayout::TRANSFER_DESTINATION,
				GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

			Engine::GetRenderer()->UploadImageToGpu(
				image.GetPointer(), 
				originalImage.image.data(), 
				numBytes, 
				uploadCmdList.GetPointer());

			uploadCmdList->SetGpuImageBarrier(
				image.GetPointer(), 
				GpuImageLayout::SAMPLED,
				GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SHADER_READ));

			delete[] data;
		}
		else if (originalImage.component == 4) {
			uploadCmdList->SetGpuImageBarrier(
				image.GetPointer(), 
				GpuImageLayout::TRANSFER_DESTINATION,
				GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

			Engine::GetRenderer()->UploadImageToGpu(
				image.GetPointer(), 
				originalImage.image.data(), 
				numBytes, 
				uploadCmdList.GetPointer());
			
			uploadCmdList->SetGpuImageBarrier(
				image.GetPointer(), 
				GpuImageLayout::SAMPLED,
				GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SHADER_READ));
		}
		else {
			OSK_ASSERT(false, UnsupportedModelImageFormatException());
		}

		uploadCmdList->Close();
		Engine::GetRenderer()->SubmitSingleUseCommandList(uploadCmdList.GetPointer());

		output.Insert(image);
	}

	return output;
}

DynamicArray<GltfMaterialInfo> IMeshLoader::LoadMaterials() {
	auto output = DynamicArray<GltfMaterialInfo>::CreateResizedArray((USize64)gltfModel.materials.size());

	for (UIndex64 i = 0; i < gltfModel.materials.size(); i++) {
		GltfMaterialInfo info{};
		info.baseColor = {
			static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.baseColorFactor[0]),
			static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.baseColorFactor[1]),
			static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.baseColorFactor[2]),
			static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.baseColorFactor[3])
		};

		info.metallicFactor = static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.metallicFactor);
		info.roughnessFactor = static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.roughnessFactor);

		if (info.roughnessFactor >= 1.0f)
			info.roughnessFactor = 1.0f;

		if (gltfModel.materials[i].pbrMetallicRoughness.baseColorTexture.index != -1) {
			info.hasBaseTexture = true;
			info.baseTextureIndex = gltfModel.textures[gltfModel.materials[i].pbrMetallicRoughness.baseColorTexture.index].source;
		}

		if (gltfModel.materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index != -1) {
			info.hasMetallicTexture = true;
			info.metallicTextureIndex = gltfModel.textures[gltfModel.materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index].source;

			Engine::GetLogger()->InfoLog("El material tiene textura metálica.");
		}

		if (gltfModel.materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index != -1) {
			info.hasRoughnessTexture = true;
			info.metallicTextureIndex = gltfModel.textures[gltfModel.materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index].source;

			Engine::GetLogger()->InfoLog("El material tiene textura de rugosidad.");
		}

		if (gltfModel.materials[i].normalTexture.index != -1) {
			info.hasNormalTexture = true;
			info.normalTextureIndex = gltfModel.textures[gltfModel.materials[i].normalTexture.index].source;
		}

		output[i] = info;
	}

	return output;
}

bool IMeshLoader::HasAttribute(const tinygltf::Primitive& primitive, const std::string& name) const {
	return primitive.attributes.contains(name);
}

bool IMeshLoader::HasPositions(const tinygltf::Primitive& primitive) const {
	return HasAttribute(primitive, "POSITION");
}

bool IMeshLoader::HasNormals(const tinygltf::Primitive& primitive) const {
	return HasAttribute(primitive, "NORMAL");
}

bool IMeshLoader::HasTangets(const tinygltf::Primitive& primitive) const {
	return HasAttribute(primitive, "TANGENT");
}

bool IMeshLoader::HasTextureCoords(const tinygltf::Primitive& primitive) const {
	return HasAttribute(primitive, "TEXCOORD_0");
}

bool IMeshLoader::HasColors(const tinygltf::Primitive& primitive) const {
	return HasAttribute(primitive, "COLOR_0");
}

bool IMeshLoader::HasJoints(const tinygltf::Primitive& primitive) const {
	return HasAttribute(primitive, "JOINTS_0");
}

bool IMeshLoader::HasBoneWeights(const tinygltf::Primitive& primitive) const {
	return HasAttribute(primitive, "WEIGHTS_0");
}

DynamicArray<Vector3f> IMeshLoader::GetVertexPositions(const tinygltf::Primitive& primitive, const glm::mat4& nodeMatrix) const {
	// Comprobamos que tiene almacenado info de posición.
	OSK_ASSERT(HasPositions(primitive), NoVertexPositionsFoundException());

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("POSITION")->second];
	const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const float* positionsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	const USize32 numVertices = static_cast<USize32>(accessor.count);
	

	// Transformamos a vértices
	DynamicArray<Vector3f> output = DynamicArray<Vector3f>::CreateResizedArray(numVertices);

	for (UIndex32 v = 0; v < numVertices; v++) {
		const glm::vec4 vertexPosition = glm::vec4(
			positionsBuffer[v * 3 + 0],
			positionsBuffer[v * 3 + 1],
			positionsBuffer[v * 3 + 2],
			globalScale
		);

		output[v] = glm::vec3(nodeMatrix * vertexPosition);
	}

	return output;
}

DynamicArray<Vector3f> IMeshLoader::GetVertexNormals(const tinygltf::Primitive& primitive) const {
	// Comprobamos que tiene almacenado info de normales.
	OSK_ASSERT(HasNormals(primitive), NoVertexNormalsFoundException());

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("NORMAL")->second];
	const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const float* normalsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	const USize32 numVertices = static_cast<USize32>(accessor.count);
	
	DynamicArray<Vector3f> output = DynamicArray<Vector3f>::CreateResizedArray(numVertices);

	for (UIndex32 v = 0; v < numVertices; v++) {
		output[v] = Vector3f(
			normalsBuffer[v * 3 + 0],
			normalsBuffer[v * 3 + 1],
			normalsBuffer[v * 3 + 2]
		).GetNormalized();
	}

	return output;
}

DynamicArray<Vector3f> IMeshLoader::GetTangentVectors(const tinygltf::Primitive& primitive) const {
	// Comprobamos que tiene almacenado info de tangentes.
	OSK_ASSERT(HasTangets(primitive), NoVertexTangentsFoundException());

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("TANGENT")->second];
	const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const float* tangentsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	const USize32 numVertices = static_cast<USize32>(accessor.count);

	DynamicArray<Vector3f> output = DynamicArray<Vector3f>::CreateResizedArray(numVertices);

	for (UIndex32 v = 0; v < numVertices; v++) {
		output[v] = Vector3f(
			tangentsBuffer[v * 4 + 0],
			tangentsBuffer[v * 4 + 1],
			tangentsBuffer[v * 4 + 2]
		);

		if (tangentsBuffer[v * 4 + 3] < 0.0f) 
			output[v] *= -1.0f;
	}

	return output;

}

DynamicArray<Vector3f> IMeshLoader::GenerateTangetVectors(const DynamicArray<Vector2f>& texCoords, const DynamicArray<Vector3f>& _positions, const DynamicArray<TIndexSize>& indices, UIndex32 indicesStartOffset) const {
	DynamicArray<Vector3f> output = DynamicArray<Vector3f>::CreateResizedArray(texCoords.GetSize());

	for (UIndex32 i = 0; i < indices.GetSize(); i += 3) {
		const GRAPHICS::TIndexSize indexA = indices[i + 0] - indicesStartOffset;
		const GRAPHICS::TIndexSize indexB = indices[i + 1] - indicesStartOffset;
		const GRAPHICS::TIndexSize indexC = indices[i + 2] - indicesStartOffset;

		const Vector2f tCoords[3] = {
			texCoords[indexA],
			texCoords[indexB],
			texCoords[indexC]
		};
		const Vector3f positions[3] = {
			_positions[indexA],
			_positions[indexB],
			_positions[indexC]
		};

		const Vector3f edge1 = positions[1] - positions[0];
		const Vector3f edge2 = positions[2] - positions[0];

		const Vector2f uvDelta1 = tCoords[1] - tCoords[0];
		const Vector2f uvDelta2 = tCoords[2] - tCoords[0];

		const float f = 1.0f / (uvDelta1.x * uvDelta2.y - uvDelta2.x * uvDelta1.y);

		const Vector3f tangent = Vector3f(
			f * (uvDelta2.y * edge1.x - uvDelta1.y * edge2.x),
			f * (uvDelta2.y * edge1.y - uvDelta1.y * edge2.y),
			f * (uvDelta2.y * edge1.Z - uvDelta1.y * edge2.Z)
		).GetNormalized();

		output[indexA] = tangent;
		output[indexB] = tangent;
		output[indexC] = tangent;
	}

	return output;
}

DynamicArray<Vector2f> IMeshLoader::GetTextureCoords(const tinygltf::Primitive& primitive) const {
	// Comprobamos que tiene almacenado info de coordenadas de texturas.
	OSK_ASSERT(HasTextureCoords(primitive), NoVertexTexCoordsFoundException());

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("TEXCOORD_0")->second];
	const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const float* texCoordsBuffer = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	const auto numVertices = static_cast<USize32>(accessor.count);

	DynamicArray<Vector2f> output = DynamicArray<Vector2f>::CreateResizedArray(numVertices);

	for (USize32 v = 0; v < numVertices; v++) {
		output[v] = Vector2f(
			texCoordsBuffer[v * 2 + 0],
			texCoordsBuffer[v * 2 + 1]
		);
	}

	return output;
}

DynamicArray<Color> IMeshLoader::GetVertexColors(const tinygltf::Primitive& primitive) const {
	if (!HasColors(primitive))
		return DynamicArray<Color>{};

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("COLOR_0")->second];
	const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const void* colorBuffer = &(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
	const auto numVertices = static_cast<USize32>(accessor.count);

	const auto colorVectorType = accessor.type;
	const auto colorDataType = accessor.componentType;

	const int stride = colorVectorType;

	DynamicArray<Color> output = DynamicArray<Color>::CreateResizedArray(numVertices);

	for (UIndex32 v = 0; v < numVertices; v++) {

		switch (colorDataType) {

		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
			constexpr float denom = std::numeric_limits<uint8_t>::max();
			output[v] = {
				static_cast<const uint8_t*>(colorBuffer)[v * stride + 0] / denom,
				static_cast<const uint8_t*>(colorBuffer)[v * stride + 1] / denom,
				static_cast<const uint8_t*>(colorBuffer)[v * stride + 2] / denom,
				1.0f
			};

			break;
		}

		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
			constexpr float denom = std::numeric_limits<uint16_t>::max();
			output[v] = {
				static_cast<const uint16_t*>(colorBuffer)[v * stride + 0] / denom,
				static_cast<const uint16_t*>(colorBuffer)[v * stride + 1] / denom,
				static_cast<const uint16_t*>(colorBuffer)[v * stride + 2] / denom,
				1.0f
			};

			break;
		}

		case TINYGLTF_PARAMETER_TYPE_FLOAT: {
			output[v] = Color{
				static_cast<const float*>(colorBuffer)[v * stride + 0],
				static_cast<const float*>(colorBuffer)[v * stride + 1],
				static_cast<const float*>(colorBuffer)[v * stride + 2],
				1.0f
			};

			break;
		}
		}
	}
}

DynamicArray<TIndexSize> IMeshLoader::GetIndices(const tinygltf::Primitive& primitive, TIndexSize startOffset) const {
	// Índices
	const tinygltf::Accessor& indicesAccesor = gltfModel.accessors[primitive.indices];
	const tinygltf::BufferView& indicesView = gltfModel.bufferViews[indicesAccesor.bufferView];
	const tinygltf::Buffer& indicesBuffer = gltfModel.buffers[indicesView.buffer];

	const auto numIndices = static_cast<USize32>(indicesAccesor.count);

	DynamicArray<TIndexSize> output = DynamicArray<TIndexSize>::CreateResizedArray(numIndices);

	// Los índices también se guardan en buffers.
	// Necesitamos saber su tipo para procesar el buffer.
	// El índice es el ID del vértice dentro de la primitiva.
	// Para obtener el ID real, debemos tener en cuenta todos los vértices anteriormente procesados.
	switch (indicesAccesor.componentType) {
		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
			const uint32_t* indBuffer = (const uint32_t*)(&indicesBuffer.data[indicesAccesor.byteOffset + indicesView.byteOffset]);
			for (UIndex32 index = 0; index < indicesAccesor.count; index++)
				output[index] = ((TIndexSize)indBuffer[index] + startOffset);

			break;
		}

		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
			const uint16_t* indBuffer = (const uint16_t*)(&indicesBuffer.data[indicesAccesor.byteOffset + indicesView.byteOffset]);
			for (UIndex32 index = 0; index < indicesAccesor.count; index++)
				output[index] = ((TIndexSize)indBuffer[index] + startOffset);

			break;
		}

		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
			const uint8_t* indBuffer = &indicesBuffer.data[indicesAccesor.byteOffset + indicesView.byteOffset];
			for (UIndex32 index = 0; index < indicesAccesor.count; index++)
				output[index] = ((TIndexSize)indBuffer[index] + startOffset);

			break;
		}

		default:
			OSK_ASSERT(false, UnsupportedIndexTypeException(std::to_string(indicesAccesor.componentType)));
			break;

	}

	return output;
}

DynamicArray<Vector4f> IMeshLoader::GetJoints(const tinygltf::Primitive& primitive) const {
	if (!HasJoints(primitive))
		return DynamicArray<Vector4f>();

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("JOINTS_0")->second];
	const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const void* boneIds = &(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
	const int jointsDataType = accessor.componentType;

	const USize32 numVertices = static_cast<USize32>(accessor.count);

	DynamicArray<Vector4f> output = DynamicArray<Vector4f>::CreateResizedArray(numVertices);

	for (UIndex32 v = 0; v < numVertices; v++) {
		switch (jointsDataType) {

		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
			output[v] = {
				static_cast<float>(static_cast<const uint32_t*>(boneIds)[v * 4 + 0]),
				static_cast<float>(static_cast<const uint32_t*>(boneIds)[v * 4 + 1]),
				static_cast<float>(static_cast<const uint32_t*>(boneIds)[v * 4 + 2]),
				static_cast<float>(static_cast<const uint32_t*>(boneIds)[v * 4 + 3])
			};

			break;
		}

		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
			output[v] = {
				static_cast<float>(static_cast<const uint16_t*>(boneIds)[v * 4 + 0]),
				static_cast<float>(static_cast<const uint16_t*>(boneIds)[v * 4 + 1]),
				static_cast<float>(static_cast<const uint16_t*>(boneIds)[v * 4 + 2]),
				static_cast<float>(static_cast<const uint16_t*>(boneIds)[v * 4 + 3])
			};

			break;
		}

		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
			output[v] = {
				static_cast<float>(static_cast<const uint8_t*>(boneIds)[v * 4 + 0]),
				static_cast<float>(static_cast<const uint8_t*>(boneIds)[v * 4 + 1]),
				static_cast<float>(static_cast<const uint8_t*>(boneIds)[v * 4 + 2]),
				static_cast<float>(static_cast<const uint8_t*>(boneIds)[v * 4 + 3])
			};

			break;
		}

		default:
			OSK_ASSERT(false, UnsupportedJointTypeException(std::to_string(jointsDataType)));
			break;

		}
	}

	return output;
}

DynamicArray<Vector4f> IMeshLoader::GetBoneWeights(const tinygltf::Primitive& primitive) const {
	if (!HasBoneWeights(primitive))
		return DynamicArray<Vector4f>();

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = gltfModel.accessors[primitive.attributes.find("WEIGHTS_0")->second];
	const tinygltf::BufferView& view = gltfModel.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const float* boneWeights = reinterpret_cast<const float*>(&(gltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));

	const USize32 numVertices = static_cast<USize32>(accessor.count);

	DynamicArray<Vector4f> output = DynamicArray<Vector4f>::CreateResizedArray(numVertices);

	for (UIndex32 v = 0; v < numVertices; v++) {
		output[v] = Vector4f(
			boneWeights[v * 4 + 0],
			boneWeights[v * 4 + 1],
			boneWeights[v * 4 + 2],
			boneWeights[v * 4 + 3]
		);
	}

	return output;
}
