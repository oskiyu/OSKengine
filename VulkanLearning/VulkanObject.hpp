#pragma once

#include <vulkan/vulkan.h>

#include "VulkanBuffer.h"
#include <vector>
#include "Vertex.hpp"
#include "PushConstants.h"
#include "VulkanImage.h";

//Objeto base de Vulkan.
struct VulkanObject {
	//Vertex buffer.
	VulkanBuffer VertexBuffer;
	//Index buffer.
	VulkanBuffer IndexBuffer;
	//Información extra que se pasa al shader.
	VulkanImage* Texture = nullptr;
	PushConst PushConstant;
	//
	std::vector<VkDescriptorSet> DescriptorSets;

	//Mesh.
	std::vector<Vertex> Vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};
	std::vector<uint16_t> Indices = {
		0, 1, 2, 2, 3, 0
	};

};

typedef VulkanObject Plane;