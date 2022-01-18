#pragma once

#include <vulkan/vulkan.h>
#include "DynamicArray.hpp"

namespace OSK {

	VkVertexInputBindingDescription GetBindingDescription_Vertex3D();

	DynamicArray<VkVertexInputAttributeDescription> GetAttributeDescription_Vertex3D();

}
