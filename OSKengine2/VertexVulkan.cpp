#include "VertexVulkan.h"

#include "Vertex.h"

VkVertexInputBindingDescription OSK::GRAPHICS::GetBindingDescription_Vertex3D() {
	VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex3D);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

OSK::DynamicArray<VkVertexInputAttributeDescription> OSK::GRAPHICS::GetAttributeDescription_Vertex3D() {
	auto output = DynamicArray<VkVertexInputAttributeDescription>::CreateResizedArray(3);

	// Posición
	output[0].binding = 0;
	output[0].location = 0;
	output[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	output[0].offset = offsetof(Vertex3D, position);

	// Color
	output[1].binding = 0;
	output[1].location = 1;
	output[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	output[1].offset = offsetof(Vertex3D, color);

	// Color
	output[2].binding = 0;
	output[2].location = 2;
	output[2].format = VK_FORMAT_R32G32_SFLOAT;
	output[2].offset = offsetof(Vertex3D, texCoords);

	return output;
}
