#include "Vertex.h"

namespace OSK {

	VkVertexInputBindingDescription Vertex::GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}


	std::array<VkVertexInputAttributeDescription, OSK_VERTEX_ATTRIBUTES_COUNT> Vertex::GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, OSK_VERTEX_ATTRIBUTES_COUNT> attributeDesc{};

		//Posición.
		attributeDesc[0].binding = 0;
		attributeDesc[0].location = 0;
		attributeDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDesc[0].offset = offsetof(Vertex, Position);

		//Color.
		attributeDesc[1].binding = 0;
		attributeDesc[1].location = 1;
		attributeDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDesc[1].offset = offsetof(Vertex, Color);

		//Texcoords.
		attributeDesc[2].binding = 0;
		attributeDesc[2].location = 2;
		attributeDesc[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDesc[2].offset = offsetof(Vertex, TextureCoordinates);

		//Normales.
		attributeDesc[3].binding = 0;
		attributeDesc[3].location = 3;
		attributeDesc[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDesc[3].offset = offsetof(Vertex, Normals);

		//Anim:
		//Weights.
		attributeDesc[4].binding = 0;
		attributeDesc[4].location = 4;
		attributeDesc[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDesc[4].offset = offsetof(Vertex, BoneWeights);

		//IDs.
		attributeDesc[5].binding = 0;
		attributeDesc[5].location = 5;
		attributeDesc[5].format = VK_FORMAT_R32G32B32A32_SINT;
		attributeDesc[5].offset = offsetof(Vertex, BondeIDs);

		return attributeDesc;
	}

}