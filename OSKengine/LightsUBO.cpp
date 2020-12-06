#include "LightsUBO.h"

#include <ext\matrix_clip_space.hpp>

namespace OSK {

	void LightUBO::UpdateBuffer(VkDevice logicalDevice, VulkanBuffer& buffer) {
		void* data;
		vkMapMemory(logicalDevice, buffer.Memory, 0, Size(), 0, &data);
		memcpy(data, &Directional, sizeof(DirectionalLight));
		memcpy(osk_add_ptr_offset(data, sizeof(DirectionalLight)), Points.data(), PointsSize());
		vkUnmapMemory(logicalDevice, buffer.Memory);
	}

}