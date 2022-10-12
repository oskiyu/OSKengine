#include "IMeshLoader.h"

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


void IMeshLoader::Load(const std::string& rawAssetPath, const glm::mat4& modelTransform) {
	this->modelTransform = modelTransform;
	this->globalScale = globalScale;

	tinygltf::TinyGLTF context;

	std::string errorMessage;
	std::string warningMessage;

	context.LoadBinaryFromFile(&gltfModel, &errorMessage, &warningMessage, rawAssetPath);
	OSK_ASSERT(errorMessage.empty(), "Error al cargar modelo estático: " + errorMessage);

	modelInfo.materialInfos = LoadMaterials();
	const tinygltf::Scene scene = gltfModel.scenes[0];

	for (TSize i = 0; i < scene.nodes.size(); i++)
		ProcessNode(gltfModel.nodes[scene.nodes[i]], scene.nodes[i], std::numeric_limits<TIndex>::max(), glm::mat4(1.0f));

	SmoothNormals();
}

void IMeshLoader::SetupModel(Model3D* model) {
	model->_SetIndexBuffer(Engine::GetRenderer()->GetMemoryAllocator()->CreateIndexBuffer(indices));
	model->_SetIndexCount(indices.GetSize());

	for (TSize i = 0; i < meshes.GetSize(); i++) {
		MeshMetadata meshMetadata{};

		if (meshIdToMaterialId.ContainsKey(i)) {
			GltfMaterialInfo& materialInfo = modelInfo.materialInfos.At(meshIdToMaterialId.Get(i));
			if (materialInfo.hasBaseTexture)
				meshMetadata.materialTextures.Insert("baseTexture", materialInfo.baseTextureIndex);

			meshMetadata.metallicFactor = materialInfo.metallicFactor;
			meshMetadata.roughnessFactor = materialInfo.roughnessFactor;
		}

		model->AddMesh(meshes.At(i), meshMetadata);
	}

	if (Engine::GetRenderer()->IsRtActive())
		model->_SetAccelerationStructure(Engine::GetRenderer()->GetMemoryAllocator()->CreateBottomAccelerationStructure(*model->GetVertexBuffer(), *model->GetIndexBuffer()));

	DynamicArray<OwnedPtr<GpuImage>> textures = LoadImages();
	for (TSize i = 0; i < textures.GetSize(); i++)
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

	for (TSize i = 0; i < gltfModel.images.size(); i++) {
		const tinygltf::Image& originalImage = gltfModel.images[i];

		auto image = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage({ (unsigned int)originalImage.width, (unsigned int)originalImage.height, 1 },
			GpuImageDimension::d2D, 1, Format::RGBA8_UNORM, GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED,
			GpuSharedMemoryType::GPU_ONLY, true);

		OwnedPtr<ICommandList> uploadCmdList = Engine::GetRenderer()->CreateSingleUseCommandList();
		uploadCmdList->Reset();
		uploadCmdList->Start();

		const TSize numBytes = originalImage.width * originalImage.height * 4;
		if (originalImage.component == 3) {
			TByte* data = (TByte*)malloc(numBytes);
			memset(data, 255, numBytes);
			OSK_ASSERT(data != NULL, "No se pudo reservar memoria para las imágenes del modelo 3d.");

			TSize rgbPos = 0;
			for (TSize i = 0; i < numBytes; i += 4) {
				memcpy(data + i, originalImage.image.data() + rgbPos, 3);
				rgbPos += 3;
			}

			uploadCmdList->SetGpuImageBarrier(image.GetPointer(), GpuImageLayout::UNDEFINED, GpuImageLayout::TRANSFER_DESTINATION,
				GpuBarrierInfo(GpuBarrierStage::DEFAULT, GpuBarrierAccessStage::DEFAULT), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE));
			Engine::GetRenderer()->UploadImageToGpu(image.GetPointer(), originalImage.image.data(), numBytes, uploadCmdList.GetPointer());
			uploadCmdList->SetGpuImageBarrier(image.GetPointer(), GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::SHADER_READ_ONLY,
				GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ));

			free(data);
		}
		else if (originalImage.component == 4) {
			uploadCmdList->SetGpuImageBarrier(image.GetPointer(), GpuImageLayout::UNDEFINED, GpuImageLayout::TRANSFER_DESTINATION,
				GpuBarrierInfo(GpuBarrierStage::DEFAULT, GpuBarrierAccessStage::DEFAULT), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE));
			Engine::GetRenderer()->UploadImageToGpu(image.GetPointer(), originalImage.image.data(), numBytes, uploadCmdList.GetPointer());
			uploadCmdList->SetGpuImageBarrier(image.GetPointer(), GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::SHADER_READ_ONLY,
				GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ));
		}
		else {
			OSK_ASSERT(false, "Formato de imagen no soportado en gltf2.0.");
		}

		uploadCmdList->Close();
		Engine::GetRenderer()->SubmitSingleUseCommandList(uploadCmdList.GetPointer());

		output.Insert(image);
	}

	return output;
}

DynamicArray<GltfMaterialInfo> IMeshLoader::LoadMaterials() {
	auto output = DynamicArray<GltfMaterialInfo>::CreateResizedArray((TSize)gltfModel.materials.size());

	for (TSize i = 0; i < gltfModel.materials.size(); i++) {
		GltfMaterialInfo info{};
		info.baseColor = {
			static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.baseColorFactor[0]),
			static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.baseColorFactor[1]),
			static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.baseColorFactor[2]),
			static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.baseColorFactor[3])
		};

		info.metallicFactor = static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.metallicFactor);
		info.roughnessFactor = static_cast<float>(gltfModel.materials[i].pbrMetallicRoughness.roughnessFactor);

		if (info.roughnessFactor == 1.0f)
			info.roughnessFactor = 0.7f;

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

		output[i] = info;
	}

	return output;
}
