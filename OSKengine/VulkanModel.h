#pragma once

#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>

#include <vulkan/vulkan.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "VulkanImage.h"
#include "Transform.h"
#include "Vertex.h"

#include "VulkanBuffer.h"

namespace OSK::VULKAN {
	
	struct VulkanDevice;

	struct VulkanMesh {
		
		std::vector<Vertex> Vertices;

		std::vector<vertexIndex_t> Indices;

	};

	struct VulkanModel {

		VulkanImage Texture;


		VulkanBuffer VertexBuffer{};


		Transform ModelTransform{};


		VulkanBuffer IndexBuffer{};


		std::vector<OSK::Vertex> Vertices;


		std::vector<vertexIndex_t> Indices;


		std::vector<VulkanMesh> Meshes;


		bool Skeletal = false;


		bool IsLoaded = false;


		void LoadModel(const VulkanDevice& device, VulkanCommandPool* commandPool, VkQueue Q, const std::string& path);


		void Draw(VulkanCommandBuffer& commandBuffer, VkPipelineLayout layout, const uint32_t& currentFrame);


		void CreateVertexBuffer(const VulkanDevice& device, VulkanCommandPool* commandPool, VkQueue Q);


		void CreateIndexBuffer(const VulkanDevice& device, VulkanCommandPool* commandPool, VkQueue Q);
		

		private:

			void processMeshes(aiNode* node, const aiScene* scene);


			void processMesh(aiMesh* mesh, const aiScene* scene);

	};

}

/*
//Vertex layout components
enum class VertexComponent {
	POSITION,
	NORMAL,
	COLOR,
	UV,
	TANGENT,
	BITANGENT,
	DUMMY_FLOAT,
	DUMMY_VEC4
};

struct VertexLayout {

public:

	std::vector<VertexComponent> components;

	VertexLayout(std::vector<VertexComponent> components) {
		this->components = std::move(components);
	}

	//Tamaño
	uint32_t stride() {
		uint32_t res = 0;
		for (auto& c : components) {
			switch (c) {
			case VertexComponent::UV:
				res += 2 * sizeof(float_t);
				break;

			case VertexComponent::DUMMY_FLOAT:
				res += sizeof(float_t);
				break;

			case VertexComponent::DUMMY_VEC4:
				res += 4 * sizeof(float_t);

			default:
				res += 3 * sizeof(float_t);
				break;
			}
		}

		return res;
	}

};

struct ModelCreateInfo {
	glm::vec3 center;
	glm::vec3 scale;
	glm::vec2 uvScale;
	VkMemoryPropertyFlags memPropFlags = 0;

	ModelCreateInfo(glm::vec3 scale, glm::vec2 uvScale, glm::vec3 center) {
		this->center = center;
		this->scale = scale;
		this->uvScale = uvScale;
	}

	ModelCreateInfo() : ModelCreateInfo(glm::vec3(1.0f), glm::vec2(1.0f), glm::vec3(0.0f)) { }

	ModelCreateInfo(float_t scale, float_t uvScale, float_t center) : ModelCreateInfo(glm::vec3(scale), glm::vec2(uvScale), glm::vec3(center)) { }

};

struct VulkanModel {
	VkDevice device = nullptr;
	Buffer vertices;
	Buffer indices;
	uint32_t indexCount = 0;
	uint32_t vertexCount = 0;

	struct ModelPart {
		uint32_t vertexBase;
		uint32_t vertexCounts;
		uint32_t indexBase;
		uint32_t indexCount;
	};
	std::vector<ModelPart> parts;

	static const int defaultFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;

	struct Dimension {
		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);
		glm::vec3 size;
	} dim;

	void destroy() {
		vkDestroyBuffer(device, vertices.buffer, nullptr);
		vkFreeMemory(device, vertices.memory, nullptr);
		if (indices.buffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(device, indices.buffer, nullptr);
			vkFreeMemory(device, indices.memory, nullptr);
		}
	}

	bool loadFromFile(const std::string& fileName, VertexLayout layout, ModelCreateInfo* createInfo, VulkanDevice* device, VkQueue copyQueue) {
		this->device = device->logicalDevice;

		static Assimp::Importer importer;
		const aiScene* pScene;

		pScene = importer.ReadFile(fileName.c_str(), defaultFlags);
		if (!pScene) {
			//exitFatal(importer.GetErrorString().c_str());

			//TODO: cleanup
			return false;
		}

		parts.clear();
		parts.resize(pScene->mNumMeshes);

		glm::vec3 scale(1.0f);
		glm::vec2 uvScale(1.0f);
		glm::vec3 center(0.0f);
		if (createInfo) {
			scale = createInfo->scale;
			uvScale = createInfo->uvScale;
			center = createInfo->center;
		}

		std::vector<float_t> vertexBuffer;
		vertexCount = 0;

		std::vector<uint32_t> indexBuffer;
		indexCount = 0;

		for (uint32_t i = 0; i < pScene->mNumMeshes; i++) {
			const aiMesh* paiMesh = pScene->mMeshes[i];

			parts[i] = { };
			parts[i].vertexBase = vertexCount;
			parts[i].indexBase = indexCount;

			vertexCount += pScene->mMeshes[i]->mNumVertices;

			aiColor3D pColor(0.0f, 0.0f, 0.0f);
			pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);

			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

			for (uint32_t j = 0; j < paiMesh->mNumVertices; j++) {
				const aiVector3D* pPos = &(paiMesh->mVertices[j]);
				const aiVector3D* pNormal = &(paiMesh->mNormals[j]);
				const aiVector3D* pTexCoords = (paiMesh->HasTextureCoords(0)) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;
				const aiVector3D* pTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mTangents[j]) : &Zero3D;
				const aiVector3D* pBitangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mBitangents[j]) : &Zero3D;

				for (auto& component : layout.components) {
					switch (component) {
						case VertexComponent::POSITION:
							vertexBuffer.push_back(pPos->x * scale.x + center.x);
							vertexBuffer.push_back(pPos->y * scale.y + center.y);
							vertexBuffer.push_back(pPos->z * scale.z + center.z);
							break;

						case VertexComponent::NORMAL:
							vertexBuffer.push_back(pNormal->x);
							vertexBuffer.push_back(-pNormal->y);
							vertexBuffer.push_back(pNormal->z);
							break;

						case VertexComponent::UV:
							vertexBuffer.push_back(pTexCoords->x * uvScale.s);
							vertexBuffer.push_back(pTexCoords->y * uvScale.t);
							break;

						case VertexComponent::COLOR:
							vertexBuffer.push_back(pColor.r);
							vertexBuffer.push_back(pColor.g);
							vertexBuffer.push_back(pColor.b);
							break;

						case VertexComponent::TANGENT:
							vertexBuffer.push_back(pTangent->x);
							vertexBuffer.push_back(pTangent->y);
							vertexBuffer.push_back(pTangent->y);
							break;

						case VertexComponent::BITANGENT:
							vertexBuffer.push_back(pBitangent->x);
							vertexBuffer.push_back(pBitangent->y);
							vertexBuffer.push_back(pBitangent->y);
							break;

						case VertexComponent::DUMMY_FLOAT:
							vertexBuffer.push_back(0.0f);
							break;

						case VertexComponent::DUMMY_VEC4:
							vertexBuffer.push_back(0.0f);
							vertexBuffer.push_back(0.0f);
							vertexBuffer.push_back(0.0f);
							vertexBuffer.push_back(0.0f);
							break;
					}
				}

				dim.max.x = fmax(pPos->x, dim.max.x);
				dim.max.x = fmax(pPos->y, dim.max.y);
				dim.max.x = fmax(pPos->z, dim.max.z);

				dim.min.x = fmin(pPos->x, dim.min.x);
				dim.min.y = fmin(pPos->y, dim.min.y);
				dim.min.z = fmin(pPos->z, dim.min.z);
			}

			dim.size = dim.max - dim.min;

			parts[i].vertexCounts = paiMesh->mNumVertices;

			uint32_t indexBase = static_cast<uint32_t>(indexBuffer.size());
			for (uint32_t j = 0; j < paiMesh->mNumFaces; j++) {
				const aiFace& face = paiMesh->mFaces[j];
				
				if (face.mNumIndices != 3)
					continue;

				indexBuffer.push_back(indexBase + face.mIndices[0]);
				indexBuffer.push_back(indexBase + face.mIndices[1]);
				indexBuffer.push_back(indexBase + face.mIndices[2]);

				parts[i].indexCount += 3;
				indexCount += 3;
			}
		}

		uint32_t vertexBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(float_t);
		uint32_t indexBufferSize = static_cast<uint32_t>(indexBuffer.size()) * sizeof(uint32_t);

		Buffer vertexStaging;
		Buffer indexStaging;

		device->createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertexStaging, vertexBufferSize, vertexBuffer.data());
		device->createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &indexStaging, indexBufferSize, indexBuffer.data());

		device->createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | createInfo->memPropFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertices, vertexBufferSize);
		device->createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | createInfo->memPropFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indices, indexBufferSize);

		VkCommandBuffer copyCmd = device->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		VkBufferCopy copyRegion = { };
		copyRegion.size = vertexBufferSize;
		vkCmdCopyBuffer(copyCmd, vertexStaging.buffer, vertices.buffer, 1, &copyRegion);

		copyRegion.size = indexBufferSize;
		vkCmdCopyBuffer(copyCmd, indexStaging.buffer, indices.buffer, 1, &copyRegion);

		device->flushCommandBuffer(copyCmd, copyQueue);

		vkDestroyBuffer(device->logicalDevice, vertexStaging.buffer, nullptr);
		vkFreeMemory(device->logicalDevice, vertexStaging.memory, nullptr);

		vkDestroyBuffer(device->logicalDevice, indexStaging.buffer, nullptr);
		vkFreeMemory(device->logicalDevice, indexStaging.memory, nullptr);

		return true;
	}

	bool loadFromFile(const std::string& fileName, VertexLayout layout, const float_t& scale, VulkanDevice* device, VkQueue copyQueue) {
		ModelCreateInfo modelCreateInfo(scale, 1.0f, 0.0f);
		
		return loadFromFile(fileName, layout, &modelCreateInfo, device, copyQueue);
	}
};
*/