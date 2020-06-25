#pragma once

#include <vulkan/vulkan.h>
#include <glm.hpp>

#include <array>

struct Vertex {
	//Position.
	glm::vec2 Position;
	//Color.
	glm::vec3 Color;

	//Atributos que se pasan a la GPU.
	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	//Atributos que se pasan a la GPU.
	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDesc{};

		//Posición.
		attributeDesc[0].binding = 0;
		attributeDesc[0].location = 0;
		attributeDesc[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDesc[0].offset = offsetof(Vertex, Position);

		//Color.
		attributeDesc[1].binding = 0;
		attributeDesc[1].location = 1;
		attributeDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDesc[1].offset = offsetof(Vertex, Color);

		return attributeDesc;
	}

};
