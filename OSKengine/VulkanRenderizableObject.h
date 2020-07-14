#pragma once

#include "Texture.h"
#include "Vertex.h"
#include "VulkanBuffer.h"
#include <glm.hpp>
#include <vector>

namespace OSK::VULKAN {

	struct OSKAPI_CALL VulkanRenderizableObject {
		//Vertex buffer.
		VulkanBuffer VertexBuffer;
		//Index buffer.
		VulkanBuffer IndexBuffer;
		//Información extra que se pasa al shader.
		OSK::Texture* Texture = nullptr;
		void* PushConst = nullptr;
		//
		std::vector<VkDescriptorSet> DescriptorSets;

		//Mesh.
		std::vector<Vertex> Vertices = {
			{{-1, -1, -1}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{1, -1, -1}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{1, 1, -1}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-1, 1, -1}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		};
		std::vector<uint16_t> Indices = {
			0, 1, 2, 2, 3, 0
		};

	};

}