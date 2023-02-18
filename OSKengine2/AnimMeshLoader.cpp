#include "AnimMeshLoader.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"

#include <ext/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Animation.h"
#include "AnimationSampler.h"
#include "Material.h"
#include "AnimationSkin.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void AnimMeshLoader::SmoothNormals() {
	for (TSize i = 0; i < indices.GetSize(); i += 3) {
		const TIndexSize localIndices[3] = {
			indices.At(i + 0),
			indices.At(i + 1),
			indices.At(i + 2)
		};

		const VertexAnim3D localVertices[3] = {
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

void AnimMeshLoader::ProcessNode(const tinygltf::Node& node, TSize nodeId, TSize parentId) {
	if (tempAnimator.GetActiveSkin() == nullptr)
		LoadSkins();

	// La matriz inicial se aplica en la animación.
	const glm::mat4 nodeMatrix = GetNodeMatrix(node);

	MeshNode animNode{};
	animNode.name = node.name;
	animNode.thisIndex = nodeId;
	animNode.parentIndex = parentId;

	if (node.skin > -1)
		animNode.skinIndex = node.skin;

	// Proceso del polígono.
	if (node.mesh > -1) {
		const tinygltf::Mesh& mesh = gltfModel.meshes[node.mesh];

		if (mesh.primitives[0].material > -1)
			meshIdToMaterialId.Insert(meshes.GetSize(), mesh.primitives[0].material);

		for (TSize i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& primitive = mesh.primitives[i];

			OSK_ASSERT(primitive.mode == TINYGLTF_MODE_TRIANGLES, "El modelo no está en modo TRIÁNGULOS.");

			const TSize firstVertexId = vertices.GetSize();
			const TSize firstIndexId = indices.GetSize();

			const auto positions = GetVertexPositions(primitive, nodeMatrix);
			const auto normals = GetVertexNormals(primitive);
			const auto texCoords = GetTextureCoords(primitive);
			const auto colors = GetVertexColors(primitive);
			const auto joints = GetJoints(primitive);
			const auto boneWeights = GetBoneWeights(primitive);

			const TSize numVertices = positions.GetSize();

			const bool hasColors = !colors.IsEmpty();
			const bool hasJoints= !joints.IsEmpty();
			const bool hasBoneWeights = !boneWeights.IsEmpty();


			// Procesamos los buffers y generamos nuevos vértices.
			for (TSize v = 0; v < numVertices; v++) {
				
				VertexAnim3D vertex{};

				vertex.position = positions[v];
				vertex.normal = normals[v];
				vertex.texCoords = texCoords[v];
				vertex.color = Color::WHITE();

				if (hasJoints)
					vertex.boneIndices = joints[v];

				if (hasBoneWeights)
					vertex.boneWeights = boneWeights[v];

				if (primitive.material > -1)
					vertex.color = modelInfo.materialInfos[primitive.material].baseColor;

				if (hasColors)
					vertex.color = colors[v];

				vertices.Insert(vertex);
			}


			const auto primitiveIndices = GetIndices(primitive, firstVertexId);
			indices.InsertAll(primitiveIndices);

			meshes.Insert(Mesh3D(primitiveIndices.GetSize(), firstIndexId));
		}
	}

	// Engine::GetLogger()->InfoLog("Nodo cargado: " + animNode.name + " índice: " + std::to_string(animNode.thisIndex) + " parent: " + std::to_string(animNode.parentIndex));

	for (TSize i = 0; i < node.children.size(); i++) {
		ProcessNode(gltfModel.nodes[node.children[i]], node.children[i], nodeId);
		animNode.childIndices.Insert(node.children[i]);
	}

	tempAnimator._AddNode(animNode);
}

void AnimMeshLoader::LoadAnimations() {
	const TSize numAnimations = gltfModel.animations.size();

	for (TSize animationId = 0; animationId < numAnimations; animationId++) {
		tinygltf::Animation gltfAnimation = gltfModel.animations[animationId];
		Animation animation{};

		animation.name = gltfAnimation.name;
		animation.samplers.Resize(gltfAnimation.samplers.size());
		animation.channels.Resize(gltfAnimation.channels.size());

		// Samplers
		for (TSize samplerId = 0; samplerId < gltfAnimation.samplers.size(); samplerId++) {
			tinygltf::AnimationSampler gltfSampler = gltfAnimation.samplers[samplerId];

			AnimationSampler& sampler = animation.samplers[samplerId];
			// TODO: sampler interpolation type

			// Establece los valores de entrada del sampler, es decir,
			// los timestamps en el que se produce una transforamción de algún tipo.
			{
				const tinygltf::Accessor& accessor = gltfModel.accessors[gltfSampler.input];
				const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];

				const void* data = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
				const float* timestampsBuffer = static_cast<const float*>(data);

				for (TSize i = 0; i < accessor.count; i++)
					sampler.timestamps.Insert(timestampsBuffer[i]);

				// Timestamps de inicio y fin de la animación.
				for (const TDeltaTime timestamp : sampler.timestamps) {
					animation.startTime = glm::min(animation.startTime, timestamp);
					animation.endTime = glm::max(animation.endTime, timestamp);
				}
			}

			// Establece los valores de salida del sampler, es decir,
			// la transformación de cada timestamp.
			{
				const tinygltf::Accessor& accessor = gltfModel.accessors[gltfSampler.output];
				const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];
				const void* data = &buffer.data[accessor.byteOffset + bufferView.byteOffset];

				switch (accessor.type) {

				case TINYGLTF_TYPE_VEC3: {
					const glm::vec3* vecBuffer = static_cast<const glm::vec3*>(data);
					for (TSize i = 0; i < accessor.count; i++)
						sampler.outputs.Insert(glm::vec4(vecBuffer[i], 0.0));
				}
					break;

				case TINYGLTF_TYPE_VEC4: {
					const glm::vec4* vecBuffer = static_cast<const glm::vec4*>(data);
					for (TSize i = 0; i < accessor.count; i++)
						sampler.outputs.Insert(vecBuffer[i]);
				}
					break;

				}
			}
		}

		// Channels
		for (TSize channelId = 0; channelId < gltfAnimation.channels.size(); channelId++) {
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
				OSK_CHECK(false, "El canal de animación " + gltfChannel.target_path + " no está soportado.");
			}
		}

		// Engine::GetLogger()->InfoLog("Animación cargada: " + animation.name);
		tempAnimator._AddAnimation(animation);
	}

}

void AnimMeshLoader::LoadSkins() {
	const TSize numSkins = gltfModel.skins.size();
	
	for (TSize skinId = 0; skinId < gltfModel.skins.size(); skinId++) {
		tinygltf::Skin& gltfSkin = gltfModel.skins[skinId];

		AnimationSkin skin;
		skin.name = gltfSkin.name;
		skin.thisIndex = skinId;

		if (gltfSkin.skeleton > -1)
			skin.rootIndex = static_cast<TIndex>(gltfSkin.skeleton);
		
		for (const TSize bone : gltfSkin.joints)
			skin.bonesIds.Insert(bone);

		if (gltfSkin.inverseBindMatrices > -1) {
			const tinygltf::Accessor& accessor = gltfModel.accessors[gltfSkin.inverseBindMatrices];
			const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];

			const TSize matricesSize = accessor.count * sizeof(glm::mat4);

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

	LoadAnimations();
	
	model->_SetAnimator(std::move(tempAnimator));
	model->GetAnimator()->Setup(modelTransform);

	IMeshLoader::SetupModel(model);
}
