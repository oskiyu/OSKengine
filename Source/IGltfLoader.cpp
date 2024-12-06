#include "IGltfLoader.h"

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
#include "StringOperations.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include "Texture.h"
#include "TextureLoader.h"
#include "AssetOwningRef.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


CpuModel3D GltfLoader::Load(std::string_view path, const glm::mat4& transform, float globalScale) {
	CpuModel3D output = {};

	tinygltf::TinyGLTF context;

	std::string errorMessage = "";
	std::string warningMessage = "";

	tinygltf::Model gltfModel{};

	context.LoadBinaryFromFile(&gltfModel, &errorMessage, &warningMessage, static_cast<std::string>(path));
	OSK_ASSERT(errorMessage.empty(), EngineException("Error al cargar modelo estático: " + errorMessage));

	for (const auto& scene : gltfModel.scenes) {
		for (const int nodeIndex : scene.nodes) {
			const tinygltf::Node& node = gltfModel.nodes[nodeIndex];
			output.AddMeshes(ProcessNode(gltfModel, node, transform, globalScale));
		}
	}

	output.AddAnimations(LoadAnimations(gltfModel));
	output.AddAnimationSkins(LoadAnimationSkins(gltfModel));

	return output;
}

void GltfLoader::LoadMaterials(std::string_view path, DynamicArray<GRAPHICS::GpuModel3D::Material>* materials, GRAPHICS::GpuModel3D::TextureTable* textures) {
	tinygltf::TinyGLTF context;

	std::string errorMessage = "";
	std::string warningMessage = "";

	tinygltf::Model gltfModel{};

	context.LoadBinaryFromFile(&gltfModel, &errorMessage, &warningMessage, static_cast<std::string>(path));
	OSK_ASSERT(errorMessage.empty(), EngineException("Error al cargar modelo estático: " + errorMessage));

	*materials = LoadMaterials(gltfModel);

	const auto images = LoadImages(gltfModel);

	for (const auto& image : images) {
		textures->AddTexture(image);
	}
}

DynamicArray<AnimationSkin> GltfLoader::LoadAnimationSkins(const tinygltf::Model& model) {
	DynamicArray<AnimationSkin> output = DynamicArray<AnimationSkin>::CreateReserved(model.skins.size());

	for (UIndex32 skinId = 0; skinId < model.skins.size(); skinId++) {
		const tinygltf::Skin& gltfSkin = model.skins[skinId];

		AnimationSkin skin;
		skin.name = gltfSkin.name;
		skin.thisIndex = skinId;

		if (gltfSkin.skeleton > -1)
			skin.rootIndex = static_cast<UIndex32>(gltfSkin.skeleton);

		for (const auto bone : gltfSkin.joints)
			skin.bonesIds.Insert(bone);

		if (gltfSkin.inverseBindMatrices > -1) {
			const tinygltf::Accessor& accessor = model.accessors[gltfSkin.inverseBindMatrices];
			const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

			const auto matricesSize = static_cast<UIndex32>(accessor.count * sizeof(glm::mat4));

			skin.inverseMatrices.Resize(accessor.count);
			memcpy(skin.inverseMatrices.GetData(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], matricesSize);
		}

		output.Insert(skin);
	}

	return output;
}

DynamicArray<GpuModel3D::Material> GltfLoader::LoadMaterials(const tinygltf::Model& model) {
	auto output = DynamicArray<GpuModel3D::Material>::CreateResized(model.materials.size());

	for (UIndex64 i = 0; i < model.materials.size(); i++) {
		GpuModel3D::Material material{};

		material.baseColor = {
			static_cast<float>(model.materials[i].pbrMetallicRoughness.baseColorFactor[0]),
			static_cast<float>(model.materials[i].pbrMetallicRoughness.baseColorFactor[1]),
			static_cast<float>(model.materials[i].pbrMetallicRoughness.baseColorFactor[2]),
			static_cast<float>(model.materials[i].pbrMetallicRoughness.baseColorFactor[3])
		};

		material.roughnessFactor = static_cast<float>(model.materials[i].pbrMetallicRoughness.roughnessFactor);
		material.metallicFactor = static_cast<float>(model.materials[i].pbrMetallicRoughness.metallicFactor);

		material.emissiveColor = {
			static_cast<float>(model.materials[i].emissiveFactor[0]),
			static_cast<float>(model.materials[i].emissiveFactor[1]),
			static_cast<float>(model.materials[i].emissiveFactor[2])
		};

		if (model.materials[i].pbrMetallicRoughness.baseColorTexture.index != -1) {
			material.colorTextureIndex = model.textures[model.materials[i].pbrMetallicRoughness.baseColorTexture.index].source;
		}

		if (model.materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index != -1) {
			material.metallicTextureIndex = model.textures[model.materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index].source;
		}

		if (model.materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index != -1) {
			material.metallicTextureIndex = model.textures[model.materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index].source;
		}

		if (model.materials[i].normalTexture.index != -1) {
			material.normalTextureIndex = model.textures[model.materials[i].normalTexture.index].source;
		}

		output[i] = material;
	}

	return output;
}

DynamicArray<Animation> GltfLoader::LoadAnimations(const tinygltf::Model& model) {
	DynamicArray<Animation> output = DynamicArray<Animation>::CreateReserved(model.animations.size());

	const auto bones = LoadAllBones(model);

	for (const auto& gltfAnimation : model.animations) {
		output.Insert(
			Animation(
				gltfAnimation.name,
				LoadAnimationSamplers(model, gltfAnimation),
				LoadAnimationChannels(gltfAnimation),
				bones));
	}

	return output;
}

DynamicArray<AnimationSampler> GltfLoader::LoadAnimationSamplers(const tinygltf::Model& model, const tinygltf::Animation& gltfAnimation) {
	DynamicArray<AnimationSampler> samplers = DynamicArray<AnimationSampler>::CreateResized(gltfAnimation.samplers.size());

	for (UIndex32 samplerId = 0; samplerId < gltfAnimation.samplers.size(); samplerId++) {
		tinygltf::AnimationSampler gltfSampler = gltfAnimation.samplers[samplerId];

		AnimationSampler& sampler = samplers[samplerId];
		// TODO: sampler interpolation type

		// Establece los valores de entrada del sampler, es decir,
		// los timestamps en el que se produce una transforamción de algún tipo.
		{
			const tinygltf::Accessor& accessor = model.accessors[gltfSampler.input];
			const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

			const void* data = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
			const auto* timestampsBuffer = static_cast<const float*>(data);

			for (UIndex32 i = 0; i < accessor.count; i++) {
				sampler.timestamps.Insert(timestampsBuffer[i]);
			}
		}

		// Establece los valores de salida del sampler, es decir,
		// la transformación de cada timestamp.
		{
			const tinygltf::Accessor& accessor = model.accessors[gltfSampler.output];
			const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
			const void* data = &buffer.data[accessor.byteOffset + bufferView.byteOffset];

			switch (accessor.type) {

			case TINYGLTF_TYPE_VEC3: {
				const glm::vec3* vecBuffer = static_cast<const glm::vec3*>(data);
				for (UIndex32 i = 0; i < accessor.count; i++)
					sampler.outputs.Insert(glm::vec4(vecBuffer[i], 0.0));
			}
								   break;

			case TINYGLTF_TYPE_VEC4: {
				const glm::vec4* vecBuffer = static_cast<const glm::vec4*>(data);
				for (UIndex32 i = 0; i < accessor.count; i++)
					sampler.outputs.Insert(vecBuffer[i]);
			}
								   break;

			}
		}
	}
	return samplers;
}

DynamicArray<AnimationChannel> GltfLoader::LoadAnimationChannels(const tinygltf::Animation& gltfAnimation) {
	DynamicArray<AnimationChannel> channels = DynamicArray<AnimationChannel>::CreateResized(gltfAnimation.channels.size());

	for (UIndex32 channelId = 0; channelId < gltfAnimation.channels.size(); channelId++) {
		tinygltf::AnimationChannel gltfChannel = gltfAnimation.channels[channelId];

		AnimationChannel& channel = channels[channelId];
		channel.samplerIndex = channelId;
		channel.nodeId = gltfChannel.target_node;

		if (gltfChannel.target_path == "translation") {
			channel.type = AnimationChannel::ChannelType::TRANSLATION;
		}
		else if (gltfChannel.target_path == "rotation") {
			channel.type = AnimationChannel::ChannelType::ROTATION;
		}
		else if (gltfChannel.target_path == "scale") {
			channel.type = AnimationChannel::ChannelType::SCALE;
		}
		else {
			// OSK_CHECK(false, "El canal de animación " + gltfChannel.target_path + " no está soportado.");
		}
	}

	return channels;
}

DynamicArray<std::string> GltfLoader::GetTagsFromName(const tinygltf::Scene& scene) {
	return Tokenize(scene.name, ':');
}

glm::mat4 GltfLoader::GetNodeMatrix(const tinygltf::Node& node) {
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

std::unordered_map<std::string, std::string, StringHasher, std::equal_to<>> GltfLoader::GetCustomProperties(const tinygltf::Mesh& mesh) {
	std::unordered_map<std::string, std::string, StringHasher, std::equal_to<>> output{};
	const auto& extras = mesh.extras;

	if (extras.IsObject()) {
		for (const auto& key : extras.Keys()) {
			const auto& value = extras.Get(key);

			switch (value.Type()) {
			case tinygltf::Type::NULL_TYPE:
				output[key] = "NULL";
				break;

			case tinygltf::Type::REAL_TYPE:
				output[key] = std::format("{}", value.GetNumberAsDouble());
				break;

			case tinygltf::Type::INT_TYPE:
				output[key] = std::format("{}", value.GetNumberAsInt());
				break;

			case tinygltf::Type::BOOL_TYPE:
				output[key] = value.Get<bool>();
				break;

			case tinygltf::Type::STRING_TYPE:
				output[key] = value.Get<std::string>();
				break;

			case tinygltf::Type::ARRAY_TYPE:
				throw UnsupportedMeshProperties("ARRAY_TYPE");
			case tinygltf::Type::BINARY_TYPE:
				throw UnsupportedMeshProperties("BINARY_TYPE");
			case tinygltf::Type::OBJECT_TYPE:
				throw UnsupportedMeshProperties("OBJECT_TYPE");
			}
		}
	}

	return output;
}

DynamicArray<AnimationBone> GltfLoader::LoadAllBones(const tinygltf::Model& model) {
	DynamicArray<AnimationBone> output{};

	for (const auto& scene : model.scenes) {
		for (const int nodeIndex : scene.nodes) {
			const tinygltf::Node& node = model.nodes[nodeIndex];
			constexpr auto noParentId = std::numeric_limits<UIndex32>::max();

			output.InsertAll(LoadBones(model, node, nodeIndex, noParentId));
		}
	}

	return output;
}

DynamicArray<AnimationBone> GltfLoader::LoadBones(const tinygltf::Model& model, const tinygltf::Node& node, UIndex64 nodeIndex, UIndex64 parentIndex) {
	DynamicArray<AnimationBone> output{};
	
	AnimationBone bone{};
	bone.name = node.name;
	bone.thisIndex = nodeIndex;
	bone.parentIndex = parentIndex;

	if (node.skin > -1) {
		bone.skinIndex = node.skin;
	}

	output.Insert(bone);

	for (UIndex32 i = 0; i < node.children.size(); i++) {
		output.InsertAll(LoadBones(model, model.nodes[node.children[i]], node.children[i], nodeIndex));
	}

	return output;
}

DynamicArray<CpuMesh3D> GltfLoader::ProcessNode(const tinygltf::Model& model, const tinygltf::Node& node, const glm::mat4& transform, float globalScale) {
	DynamicArray<CpuMesh3D> output = {};

	const glm::mat4 nodeMatrix = transform * GetNodeMatrix(node);
	const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(nodeMatrix)));

	if (node.mesh <= -1) {
		for (UIndex32 i = 0; i < node.children.size(); i++) {
			output.InsertAll(ProcessNode(model, model.nodes[node.children[i]], transform, globalScale));
		}

		return output;
	}

	// Proceso del polígono.
	const tinygltf::Mesh& mesh = model.meshes[node.mesh];

	for (UIndex32 i = 0; i < mesh.primitives.size(); i++) {
		CpuMesh3D cpuMesh{};

		const tinygltf::Primitive& primitive = mesh.primitives[i];

		if (primitive.material > -1) {
			cpuMesh.SetMaterialIndex(primitive.material);
		}

		const auto primitiveIndices = GetIndices(primitive, 0, model);

		const std::optional<DynamicArray<Vector3f>> positions = HasPositions(primitive)
			? GetVertexPositions(primitive, nodeMatrix, model, globalScale)
			: std::optional<DynamicArray<Vector3f>>{};

		const std::optional<DynamicArray<Vector3f>> normals = HasNormals(primitive)
			? GetVertexNormals(primitive, model)
			: std::optional<DynamicArray<Vector3f>>{};

		const std::optional<DynamicArray<Vector2f>> texCoords = HasTextureCoords(primitive)
			? GetTextureCoords(primitive, model)
			: std::optional<DynamicArray<Vector2f>>{};

		const std::optional<DynamicArray<Color>> colors = HasColors(primitive)
			? GetVertexColors(primitive, model)
			: std::optional<DynamicArray<Color>>{};

		const std::optional<DynamicArray<Vector3f>> tangents = HasTangets(primitive)
			? GetTangentVectors(primitive, model)
			: std::optional<DynamicArray<Vector3f>>{};

		const std::optional<DynamicArray<Vector4f>> boneWeights = HasBoneWeights(primitive)
			? GetBoneWeights(primitive, model)
			: std::optional<DynamicArray<Vector4f>>{};

		const std::optional<DynamicArray<Vector4f>> boneIds = HasJoints(primitive)
			? GetJoints(primitive, model)
			: std::optional<DynamicArray<Vector4f>>{};

		const auto numVertices = positions.has_value()
			? positions.value().GetSize()
			: 0;

		// Procesamos los buffers y generamos nuevos vértices.
		for (UIndex64 v = 0; v < numVertices; v++) {

			CpuVertex3D vertex{};
			vertex.position = positions.value()[v];

			if (normals.has_value()) {
				vertex.normal = Vector3f(glm::normalize(normalMatrix * normals.value()[v].ToGlm()));
			}

			if (texCoords.has_value()) {
				vertex.uv = texCoords.value()[v];
			}

			if (tangents.has_value()) {
				vertex.tangent = Vector3f(glm::normalize(normalMatrix * tangents.value()[v].ToGlm()));
			}

			if (colors.has_value()) {
				vertex.color = colors.value()[v];
			}
			else if (primitive.material > -1) {
				const auto& nativeColor = model.materials[primitive.material].pbrMetallicRoughness.baseColorFactor;

				vertex.color = Color(
					static_cast<float>(nativeColor[0]),
					static_cast<float>(nativeColor[1]),
					static_cast<float>(nativeColor[2]));
			}

			if (boneWeights.has_value()) {
				const auto& weights = boneWeights.value()[v];
				vertex.boneWeights = {
					weights.x, weights.y, weights.z, weights.w
				};
			}

			if (boneIds.has_value()) {
				const auto& ids = boneIds.value()[v];
				vertex.boneIds = {
					ids.x, ids.y, ids.z, ids.w
				};
			}

			cpuMesh.AddVertex(vertex);
		}

		switch (primitive.mode) {
		case TINYGLTF_MODE_TRIANGLES: {
			for (UIndex64 idx = 0; idx < primitiveIndices.GetSize(); idx += 3) {
				cpuMesh.AddTriangleIndex({
					primitiveIndices[idx + 0],
					primitiveIndices[idx + 1],
					primitiveIndices[idx + 2] });
			}
		}
			break;

		case TINYGLTF_MODE_LINE: {
			for (UIndex64 idx = 0; idx < primitiveIndices.GetSize(); idx += 2) {
				cpuMesh.AddLineIndex({
					primitiveIndices[idx + 0],
					primitiveIndices[idx + 1] });
			}
		}
			break;

		case TINYGLTF_MODE_POINTS: {
			for (UIndex64 idx = 0; idx < primitiveIndices.GetSize(); idx++) {
				cpuMesh.AddPointIndex(primitiveIndices[idx]);
			}
		}
			break;
		}

		output.Insert(cpuMesh);
	}

	for (UIndex32 i = 0; i < node.children.size(); i++) {
		output.InsertAll(ProcessNode(model, model.nodes[node.children[i]], transform, globalScale));
	}

	return output;
}

DynamicArray<AssetRef<Texture>> GltfLoader::LoadImages(const tinygltf::Model& model) {
	auto* textureLoader = Engine::GetAssetManager()->GetLoader<TextureLoader>();

	DynamicArray<AssetRef<Texture>> output;

	/// @todo Single upload cmd list.

	for (UIndex32 i = 0; i < model.images.size(); i++) {
		const tinygltf::Image& originalImage = model.images[i];

		OwnedPtr<GpuImage> image = Engine::GetRenderer()->GetAllocator()->CreateImage(
			GpuImageCreateInfo::CreateDefault2D({ 
				(unsigned int)originalImage.width, 
				(unsigned int)originalImage.height },
			Format::RGBA8_UNORM,
			GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::SAMPLED)
		);
		image->SetDebugName(std::format("Mesh texture {}", i));

		// Al efectuar mip-mapping, debe usar GpuQueueType::MAIN.
		OwnedPtr<ICommandList> uploadCmdList = Engine::GetRenderer()->CreateSingleUseCommandList(GpuQueueType::MAIN); 
		uploadCmdList->Reset();
		uploadCmdList->Start();

		const USize64 numBytes = originalImage.width * originalImage.height * 4;
		if (originalImage.component == 3) {
			auto dataContainer = DynamicArray<TByte>::CreateReserved(numBytes);
			auto* data = dataContainer.GetData();

			memset(data, 255, numBytes);

			UIndex64 rgbPos = 0;
			for (UIndex64 i = 0; i < numBytes; i += 4) {
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

		AssetOwningRef<Texture> owningRef("XD");
		owningRef.GetAsset()->_SetImage(image);
		owningRef.GetAsset()->_SetSize(image->GetSize2D());
		owningRef.GetAsset()->_SetNumberOfChannels(4);
		owningRef._SetAsLoaded();

		output.Insert(owningRef.CreateRef());

		textureLoader->RegisterTexture(std::move(owningRef));
	}

	return output;
}

bool GltfLoader::HasAttribute(const tinygltf::Primitive& primitive, const std::string& name) {
	return primitive.attributes.contains(name);
}

bool GltfLoader::HasPositions(const tinygltf::Primitive& primitive) {
	return HasAttribute(primitive, "POSITION");
}

bool GltfLoader::HasNormals(const tinygltf::Primitive& primitive) {
	return HasAttribute(primitive, "NORMAL");
}

bool GltfLoader::HasTangets(const tinygltf::Primitive& primitive) {
	return HasAttribute(primitive, "TANGENT");
}

bool GltfLoader::HasTextureCoords(const tinygltf::Primitive& primitive) {
	return HasAttribute(primitive, "TEXCOORD_0");
}

bool GltfLoader::HasColors(const tinygltf::Primitive& primitive) {
	return HasAttribute(primitive, "COLOR_0");
}

bool GltfLoader::HasJoints(const tinygltf::Primitive& primitive) {
	return HasAttribute(primitive, "JOINTS_0");
}

bool GltfLoader::HasBoneWeights(const tinygltf::Primitive& primitive) {
	return HasAttribute(primitive, "WEIGHTS_0");
}

DynamicArray<Vector3f> GltfLoader::GetVertexPositions(const tinygltf::Primitive& primitive, const glm::mat4& nodeMatrix, const tinygltf::Model& model, float globalScale) {
	// Comprobamos que tiene almacenado info de posición.
	OSK_ASSERT(HasPositions(primitive), NoVertexPositionsFoundException());

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("POSITION")->second];
	const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const float* positionsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	const USize32 numVertices = static_cast<USize32>(accessor.count);
	

	// Transformamos a vértices
	DynamicArray<Vector3f> output = DynamicArray<Vector3f>::CreateResized(numVertices);

	for (UIndex32 v = 0; v < numVertices; v++) {
		const glm::vec4 vertexPosition = glm::vec4(
			positionsBuffer[v * 3 + 0],
			positionsBuffer[v * 3 + 1],
			positionsBuffer[v * 3 + 2],
			globalScale
		);

		output[v] = Vector3f(glm::vec3(nodeMatrix * vertexPosition));
	}

	return output;
}

DynamicArray<Vector3f> GltfLoader::GetVertexNormals(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
	// Comprobamos que tiene almacenado info de normales.
	OSK_ASSERT(HasNormals(primitive), NoVertexNormalsFoundException());

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("NORMAL")->second];
	const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
	
	// Leemos el buffer.
	const float* normalsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	const USize64 numVertices = accessor.count;
	
	DynamicArray<Vector3f> output = DynamicArray<Vector3f>::CreateResized(numVertices);

	for (UIndex64 v = 0; v < numVertices; v++) {
		output[v] = Vector3f(
			normalsBuffer[v * 3 + 0],
			normalsBuffer[v * 3 + 1],
			normalsBuffer[v * 3 + 2]
		).GetNormalized();
	}

	return output;
}

DynamicArray<Vector3f> GltfLoader::GetTangentVectors(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
	// Comprobamos que tiene almacenado info de tangentes.
	OSK_ASSERT(HasTangets(primitive), NoVertexTangentsFoundException());

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("TANGENT")->second];
	const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const float* tangentsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	const USize64 numVertices = accessor.count;

	DynamicArray<Vector3f> output = DynamicArray<Vector3f>::CreateResized(numVertices);

	for (UIndex64 v = 0; v < numVertices; v++) {
		const float normalizer = tangentsBuffer[v * 4 + 3] < 0.0f
			? -1.0f
			:  1.0f;

		output[v] = Vector3f(
			tangentsBuffer[v * 4 + 0] * normalizer,
			tangentsBuffer[v * 4 + 1] * normalizer,
			tangentsBuffer[v * 4 + 2] * normalizer
		);
	}

	return output;

}

DynamicArray<Vector3f> GltfLoader::GenerateTangetVectors(const DynamicArray<Vector2f>& texCoords, const DynamicArray<Vector3f>& _positions, const DynamicArray<TIndexSize>& indices, UIndex32 indicesStartOffset) {
	DynamicArray<Vector3f> output = DynamicArray<Vector3f>::CreateResized(texCoords.GetSize());

	for (UIndex32 i = 0; i < indices.GetSize(); i += 3) {
		const GRAPHICS::TIndexSize indexA = indices[i + 0] - indicesStartOffset;
		const GRAPHICS::TIndexSize indexB = indices[i + 1] - indicesStartOffset;
		const GRAPHICS::TIndexSize indexC = indices[i + 2] - indicesStartOffset;

		const std::array<const Vector2f, 3> tCoords = {
			texCoords[indexA],
			texCoords[indexB],
			texCoords[indexC]
		};
		const std::array<const Vector3f, 3> positions = {
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
			f * (uvDelta2.y * edge1.z - uvDelta1.y * edge2.z)
		).GetNormalized();

		output[indexA] = tangent;
		output[indexB] = tangent;
		output[indexC] = tangent;
	}

	return output;
}

DynamicArray<Vector2f> GltfLoader::GetTextureCoords(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
	// Comprobamos que tiene almacenado info de coordenadas de texturas.
	OSK_ASSERT(HasTextureCoords(primitive), NoVertexTexCoordsFoundException());

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
	const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const float* texCoordsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	const auto numVertices = static_cast<USize32>(accessor.count);

	DynamicArray<Vector2f> output = DynamicArray<Vector2f>::CreateResized(numVertices);

	for (USize32 v = 0; v < numVertices; v++) {
		output[v] = Vector2f(
			texCoordsBuffer[v * 2 + 0],
			texCoordsBuffer[v * 2 + 1]
		);
	}

	return output;
}

DynamicArray<Color> GltfLoader::GetVertexColors(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
	if (!HasColors(primitive))
		return DynamicArray<Color>{};

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
	const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const void* colorBuffer = &(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
	const auto numVertices = static_cast<USize32>(accessor.count);

	const auto colorVectorType = accessor.type;
	const auto colorDataType = accessor.componentType;

	const int stride = colorVectorType;

	DynamicArray<Color> output = DynamicArray<Color>::CreateResized(numVertices);

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

DynamicArray<TIndexSize> GltfLoader::GetIndices(const tinygltf::Primitive& primitive, TIndexSize startOffset, const tinygltf::Model& model) {
	// Índices
	const tinygltf::Accessor& indicesAccesor = model.accessors[primitive.indices];
	const tinygltf::BufferView& indicesView = model.bufferViews[indicesAccesor.bufferView];
	const tinygltf::Buffer& indicesBuffer = model.buffers[indicesView.buffer];

	const auto numIndices = static_cast<USize32>(indicesAccesor.count);

	DynamicArray<TIndexSize> output = DynamicArray<TIndexSize>::CreateResized(numIndices);

	// Los índices también se guardan en buffers.
	// Necesitamos saber su tipo para procesar el buffer.
	// El índice es el ID del vértice dentro de la primitiva.
	// Para obtener el ID real, debemos tener en cuenta todos los vértices anteriormente procesados.
	switch (indicesAccesor.componentType) {
		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
			const uint32_t* indBuffer = (const uint32_t*)(&indicesBuffer.data[indicesAccesor.byteOffset + indicesView.byteOffset]);
			for (UIndex32 index = 0; index < indicesAccesor.count; index++)
				output[index] = (indBuffer[index] + startOffset);

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

DynamicArray<Vector4f> GltfLoader::GetJoints(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
	if (!HasJoints(primitive))
		return DynamicArray<Vector4f>();

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("JOINTS_0")->second];
	const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const void* boneIds = &(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
	const int jointsDataType = accessor.componentType;

	const auto numVertices = static_cast<USize32>(accessor.count);

	DynamicArray<Vector4f> output = DynamicArray<Vector4f>::CreateResized(numVertices);

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

DynamicArray<Vector4f> GltfLoader::GetBoneWeights(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
	if (!HasBoneWeights(primitive))
		return DynamicArray<Vector4f>();

	// Para poder acceder a la información en forma de buffer.
	const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
	const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];

	// Leemos el buffer.
	const float* boneWeights = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));

	const USize32 numVertices = static_cast<USize32>(accessor.count);

	DynamicArray<Vector4f> output = DynamicArray<Vector4f>::CreateResized(numVertices);

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
