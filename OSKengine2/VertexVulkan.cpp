#include "VertexVulkan.h"

#include "Vertex.h"

VkVertexInputBindingDescription OSK::GetBindingDescription_Vertex3D() {
	VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex3D);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

OSK::DynamicArray<VkVertexInputAttributeDescription> OSK::GetAttributeDescription_Vertex3D() {
	auto output = DynamicArray<VkVertexInputAttributeDescription>::CreateResizedArray(2);

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

	return output;
}
