#include "AnimMeshLoader.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Animation.h"
#include "AnimationSampler.h"
#include "Material.h"
#include "AnimationSkin.h"

#include "ModelLoadingExceptions.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>


using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void AnimMeshLoader::ProcessNode(const tinygltf::Model& model, const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) {
	if (tempAnimator.GetActiveSkin() == nullptr)
		LoadSkins(model);

	// La matriz inicial se aplica en la animaci�n.
	const glm::mat4 nodeMatrix = GetNodeMatrix(node);
	const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(nodeMatrix)));

	MeshNode animNode{};
	animNode.name = node.name;
	animNode.thisIndex = nodeId;
	animNode.parentIndex = parentId;

	if (node.skin > -1)
		animNode.skinIndex = node.skin;

	// Proceso del pol�gono.
	if (node.mesh > -1) {
		const tinygltf::Mesh& mesh = model.meshes[node.mesh];

		if (mesh.primitives[0].material > -1)
			m_meshIdToMaterialId[static_cast<UIndex32>(m_meshes.GetSize())] = mesh.primitives[0].material;

		float radius = std::numeric_limits<float>::lowest();

		for (UIndex32 i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& primitive = mesh.primitives[i];

			OSK_ASSERT(primitive.mode == TINYGLTF_MODE_TRIANGLES, UnsupportedPolygonModeException(std::to_string(primitive.mode)));

			const UIndex32 firstVertexId = static_cast<UIndex32>(vertices.GetSize());
			const UIndex32 firstIndexId = static_cast<UIndex32>(m_indices.GetSize());

			const auto primitiveIndices = GetIndices(primitive, firstVertexId, model);

			const auto positions = GetVertexPositions(primitive, nodeMatrix, model);
			const auto normals = GetVertexNormals(primitive, model);
			const auto texCoords = GetTextureCoords(primitive, model);
			const auto colors = GetVertexColors(primitive, model);
			const auto joints = GetJoints(primitive, model);
			const auto boneWeights = GetBoneWeights(primitive, model);

			for (const auto& vertex : positions) {
				const glm::vec4 position = glm::inverse(nodeMatrix) * glm::vec4(vertex.ToGlm(), 1.0);
				radius = glm::max(radius, vertex.GetDistanceTo(Vector3f(glm::vec3(position) / position.w)));
			}

			const auto tangents = HasTangets(primitive)
				? GetTangentVectors(primitive, model)
				: GenerateTangetVectors(texCoords, positions, primitiveIndices, firstVertexId);

			const USize32 numVertices = static_cast<USize32>(positions.GetSize());

			const bool hasColors = !colors.IsEmpty();
			const bool hasJoints= !joints.IsEmpty();
			const bool hasBoneWeights = !boneWeights.IsEmpty();


			// Procesamos los buffers y generamos nuevos v�rtices.
			for (UIndex32 v = 0; v < numVertices; v++) {
				
				VertexAnim3D vertex{};

				vertex.position = positions[v];
				vertex.normal = Vector3f(glm::normalize(normalMatrix * normals[v].ToGlm()));
				vertex.texCoords = texCoords[v];
				vertex.color = Color::White;
				vertex.tangent = Vector3f(glm::normalize(normalMatrix * tangents[v].ToGlm()));

				if (hasJoints)
					vertex.boneIndices = joints[v];

				if (hasBoneWeights)
					vertex.boneWeights = boneWeights[v];

				if (primitive.material > -1)
					vertex.color = m_modelInfo.materialInfos[primitive.material].baseColor;

				if (hasColors)
					vertex.color = colors[v];

				vertices.Insert(vertex);


				// Attributes
				m_loadedVertices.AddVertexAttribute<VertexPositionAttribute3D>({ .position = vertex.position });
				m_loadedVertices.AddVertexAttribute<VertexAttributes3D>({
					.normal = vertex.normal,
					.color = vertex.color,
					.texCoords = vertex.texCoords,
					.tangent = vertex.tangent });
				m_loadedVertices.AddVertexAttribute<VertexAnimationAttributes3D>({
					.boneIndices = vertex.boneIndices,
					.boneWeights = vertex.boneWeights });
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
	}

	// Engine::GetLogger()->InfoLog("Nodo cargado: " + animNode.name + " �ndice: " + std::to_string(animNode.thisIndex) + " parent: " + std::to_string(animNode.parentIndex));

	for (UIndex32 i = 0; i < node.children.size(); i++) {
		ProcessNode(model, model.nodes[node.children[i]], node.children[i], nodeId);
		animNode.childIndices.Insert(node.children[i]);
	}

	tempAnimator._AddNode(animNode);
}

void AnimMeshLoader::LoadAnimations(const tinygltf::Model& model) {
	for (UIndex32 animationId = 0; animationId < model.animations.size(); animationId++) {
		tinygltf::Animation gltfAnimation = model.animations[animationId];
		Animation animation{};

		animation.name = gltfAnimation.name;
		animation.samplers.Resize(gltfAnimation.samplers.size());
		animation.channels.Resize(gltfAnimation.channels.size());

		// Samplers
		for (UIndex32 samplerId = 0; samplerId < gltfAnimation.samplers.size(); samplerId++) {
			tinygltf::AnimationSampler gltfSampler = gltfAnimation.samplers[samplerId];

			AnimationSampler& sampler = animation.samplers[samplerId];
			// TODO: sampler interpolation type

			// Establece los valores de entrada del sampler, es decir,
			// los timestamps en el que se produce una transforamci�n de alg�n tipo.
			{
				const tinygltf::Accessor& accessor = model.accessors[gltfSampler.input];
				const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

				const void* data = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
				const float* timestampsBuffer = static_cast<const float*>(data);

				for (UIndex32 i = 0; i < accessor.count; i++)
					sampler.timestamps.Insert(timestampsBuffer[i]);

				// Timestamps de inicio y fin de la animaci�n.
				for (const TDeltaTime timestamp : sampler.timestamps) {
					animation.startTime = glm::min(animation.startTime, timestamp);
					animation.endTime = glm::max(animation.endTime, timestamp);
				}
			}

			// Establece los valores de salida del sampler, es decir,
			// la transformaci�n de cada timestamp.
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

		// Channels
		for (UIndex32 channelId = 0; channelId < gltfAnimation.channels.size(); channelId++) {
			tinygltf::AnimationChannel gltfChannel = gltfAnimation.channels[channelId];

			AnimationChannel& channel = animation.channels[channelId];
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
				// OSK_CHECK(false, "El canal de animaci�n " + gltfChannel.target_path + " no est� soportado.");
			}
		}

		// Engine::GetLogger()->InfoLog("Animaci�n cargada: " + animation.name);
		tempAnimator._AddAnimation(animation);
	}

}

void AnimMeshLoader::LoadSkins(const tinygltf::Model& model) {
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

			const UIndex32 matricesSize = static_cast<UIndex32>(accessor.count * sizeof(glm::mat4));

			skin.inverseMatrices.Resize(accessor.count);
			memcpy(skin.inverseMatrices.GetData(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], matricesSize);
		}

		// Engine::GetLogger()->InfoLog("Skin cargada: " + skin.name + ", root: " + std::to_string(skin.rootIndex));
		tempAnimator._AddSkin(std::move(skin));
		if (skinId == 0)
			tempAnimator.SetActiveSkin(gltfSkin.name);
	}
	
}

void AnimMeshLoader::SetupModel(Model3D* model) {
	model->_SetVertexBuffer(Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(vertices, VertexAnim3D::GetVertexInfo()));

	// LoadAnimations();
	
	model->_SetAnimator(std::move(tempAnimator));
	model->GetAnimator()->Setup(m_modelTransform);

	IGltfLoader::SetupModel(model);
}
