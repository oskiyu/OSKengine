#include "Vertex.h"

namespace OSK {

	void Vertex::AddBoneData(const uint32_t& boneID, const float_t& boneWeight) {
		for (uint32_t i = 0; i < __VERTEX::MAX_BONES_AMOUNT; i++) {
			if (BoneWeights[i] == 0.0) {
				BoneID[i] = boneID;
				BoneWeights[i] = boneWeight;

				return;
			}
		}
	}

	VkVertexInputBindingDescription Vertex::GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 3> Vertex::GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		//(0) = position
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		//(1) = normal
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Normal);

		//(2) = texcoords
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, TexCoords);

		/*//(3) = bones
		attributeDescriptions[2].binding = 0; TODO
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, BoneID);*/

		return attributeDescriptions;
	}

}