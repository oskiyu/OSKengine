#include "VulkanModel.h"

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"

namespace OSK::VULKAN {

	void VulkanModel::LoadModel(const VulkanDevice& device, VulkanCommandPool* commandPool, VkQueue Q, const std::string& path) {
		if (IsLoaded) {
			Meshes.clear();
		}

		auto direct = path.substr(0, path.find_last_of('/'));
		//LoadTexture(model.Diffuse, direct + "/td");
		//LoadTexture(model.Specular, direct + "/ts");

		static Assimp::Importer importer;

		auto scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "cargar modelo " + path, __LINE__);
			return;
		}

		std::vector<aiNode*> nodes;
		//addNode(scene->mRootNode, nodes);

		std::vector<aiNodeAnim*> nodesAnim;
		if (scene->mNumAnimations > 0) {
			for (int32_t i = 0; i < scene->mAnimations[0]->mNumChannels; i++) {
				nodesAnim.push_back(scene->mAnimations[0]->mChannels[i]);
			}
		}

		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			for (uint32_t b = 0; b < scene->mMeshes[i]->mNumBones; b++) {
				/*	Skeletal = true;

					auto aibone = scene->mMeshes[i]->mBones[b];
					std::string boneName = aibone->mName.data;

					auto from = aibone->mOffsetMatrix;
					glm::mat4 to;

					to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
					to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
					to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
					to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

					Transform transform = Transform(to);

					Bone bone = Bone(model.Skeleton.Bones.size(), boneName);
					bone.Transform = transform;

					for (uint32_t j = 0; j < nodes.size(); j++) {
						if (nodes[j]->mName.data == boneName) {
							bone.AINode = nodes[j];
						}
					}

					for (uint32_t j = 0; j < nodesAnim.size(); j++) {
						if (nodesAnim[j]->mNodeName.data == boneName) {
							bone.AINodeAnim = nodesAnim[j];
						}
					}
					TODO
					model.Skeleton.Bones.push_back(bone);*/
			}
		}

		/*	if (Skeletal) {
				for (uint32_t i = 0; i < Skeleton.Bones.size(); i++) {
					if (model.Skeleton.Bones[i].AINode == nullptr) {
						Log(LogMessageLevels::BAD_ERROR, "nulltpr: AINode: " + std::to_string(i), __LINE__);
						continue;
					}
					if (model.Skeleton.Bones[i].AINodeAnim == nullptr) {
						Log(LogMessageLevels::BAD_ERROR, "AINodeAnim: " + std::to_string(i), __LINE__);
						continue;
					}

					auto parentName = model.Skeleton.Bones[i].AINode->mParent->mName.data;
					for (uint32_t n = 0; n < model.Skeleton.Bones.size(); n++) {
						if (model.Skeleton.Bones[n].Name == parentName) {
							model.Skeleton.Bones[i].Transform.AttachTo(&model.Skeleton.Bones[n].Transform);
							break;
						}
					}
				}
			}
			*/
		processMeshes(scene->mRootNode, scene);

		/*		if (model.Skeletal) {
					model.Skeleton.Update(0.0f);
				}
				*/
		CreateVertexBuffer(device, commandPool, Q);
		CreateIndexBuffer(device, commandPool, Q);

		IsLoaded = true;

#ifdef OSK_USE_INFO_LOGS
		Logger::Log(LogMessageLevels::INFO, "Modelo cargado: " + path);
#endif
	}


	void VulkanModel::Draw(VulkanCommandBuffer& commandBuffer, VkPipelineLayout layout, const uint32_t& currentFrame) {
		//##//
		VkBuffer vertexBuffers[] = { VertexBuffer.Buffer };
		VkDeviceSize offsets[] = { 0 };

		//Render
		vkCmdBindVertexBuffers(commandBuffer.CommandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer.CommandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
		//vkCmdBindDescriptorSets(commandBuffer.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &Texture.DescriptorSet.DescriptorSets[currentFrame], 0, nullptr);
		vkCmdDrawIndexed(commandBuffer.CommandBuffer, static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0); //vertex count, instance count, first vertex, first instance
	}


	void VulkanModel::CreateVertexBuffer(const VulkanDevice& device, VulkanCommandPool* commandPool, VkQueue Q) {
		//Crea el buffer
		VkDeviceSize bufferSize = sizeof(Vertices[0]) * Vertices.size();

		//Crea el staging buffer
		VulkanBuffer stagingBuffer{};
		stagingBuffer.CreateBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


		//Pasa la información al buffer
		void* data;
		vkMapMemory(device.LogicalDevice, stagingBuffer.BufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, Vertices.data(), (size_t)bufferSize); //~memory copy
		vkUnmapMemory(device.LogicalDevice, stagingBuffer.BufferMemory);

		VertexBuffer.CreateBuffer(device, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		//Copia el buffer
		VertexBuffer.CopyBufferFrom(device, commandPool, stagingBuffer, bufferSize, Q);

		//Cleanup
		stagingBuffer.DestroyBuffer(device);
	}


	void VulkanModel::CreateIndexBuffer(const VulkanDevice& device, VulkanCommandPool* commandPool, VkQueue Q) {
		VkDeviceSize bufferSize = sizeof(Indices[0]) * Indices.size();

		VulkanBuffer stagingBuffer{};
		stagingBuffer.CreateBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data;
		vkMapMemory(device.LogicalDevice, stagingBuffer.BufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, Indices.data(), (size_t)bufferSize);
		vkUnmapMemory(device.LogicalDevice, stagingBuffer.BufferMemory);

		IndexBuffer.CreateBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		IndexBuffer.CopyBufferFrom(device, commandPool, stagingBuffer, bufferSize, Q);

		stagingBuffer.DestroyBuffer(device);
	}


	void VulkanModel::processMeshes(aiNode* node, const aiScene* scene) {
		for (uint32_t i = 0; i < node->mNumMeshes; i++) {
			processMesh(scene->mMeshes[node->mMeshes[i]], scene);
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			processMeshes(node->mChildren[i], scene);
		}
	}


	void VulkanModel::processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<vertexIndex_t> indices;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			Vector3 vector;

			if (mesh->mVertices != nullptr) {
				vector.X = mesh->mVertices[i].x;
				vector.Y = mesh->mVertices[i].y;
				vector.Z = mesh->mVertices[i].z;
			}
			else {
				Logger::Log(LogMessageLevels::WARNING, "vértices", __LINE__);
				vector = Vector3(0.0f);
			}
			vertex.Position = vector;

			if (mesh->mNormals != nullptr) {
				vector.X = mesh->mNormals[i].x;
				vector.Y = mesh->mNormals[i].y;
				vector.Z = mesh->mNormals[i].z;
			}
			else {
				Logger::Log(LogMessageLevels::WARNING, "normales", __LINE__);
				vector = Vector3(0.0f);
			}
			vertex.Normal = vector;

			if (mesh->mTextureCoords[0]) {
				Vector2 vec;
				vec.X = mesh->mTextureCoords[0][i].x * 1;
				vec.Y = mesh->mTextureCoords[0][i].y * 1;
				vertex.TexCoords = vec;
			}
			else {
				vertex.TexCoords = Vector2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		/*if (model.Skeletal) {
			for (uint32_t i = 0; i < mesh->mNumBones; i++) {
				auto aibone = mesh->mBones[i];

				for (uint32_t j = 0; j < aibone->mNumWeights; j++) {
					aiVertexWeight vertexWeight = aibone->mWeights[j];

					int32_t startVertexID = vertexWeight.mVertexId;

					for (uint32_t k = 0; k < OSK::__VERTEX::MAX_BONES_AMOUNT; k++) {
						if (vertices[startVertexID].BoneWeights[k] == 0.0f) {
							for (uint32_t l = 0; l < model.Skeleton.Bones.size(); l++) {
								if (model.Skeleton.Bones[l].Name == aibone->mName.data) {
									vertices[startVertexID].BoneID[k] = model.Skeleton.Bones[l].ID;
								}
							}

							vertices[startVertexID].BoneWeights[k] = vertexWeight.mWeight;

							break;
						}

						if (k >= OSK::__VERTEX::MAX_BONES_AMOUNT - 1) {
							break;
						}
					}
				}
			}
		}*/

		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		VulkanMesh oskMesh = { vertices, indices };
		this->Vertices = vertices;
		this->Indices = indices;
		Meshes.push_back(oskMesh);
	}

}