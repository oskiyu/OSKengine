#pragma once

#include <vulkan/vulkan.h>

#include "VulkanBuffer.h"
#include <vector>
#include "Vertex.hpp"
#include "UBO.h"

struct VulkanObject {
	//Vertex buffer.
	VulkanBuffer VertexBuffer;
	//Index buffer.
	VulkanBuffer IndexBuffer;
	//CommandBuffers.
	std::vector<VkCommandBuffer> CommandBuffers;
	//UBOs.
	//

	const std::vector<Vertex> Vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> Indices = {
		0, 1, 2, 2, 3, 0
	};

};
