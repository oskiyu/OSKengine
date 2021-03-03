#include "LightsUBO.h"

#include <ext\matrix_clip_space.hpp>

namespace OSK {

	void LightUBO::UpdateBuffer(VkDevice logicalDevice, VulkanBuffer& buffer) {
		buffer.MapMemory();
		buffer.WriteMapped(&Directional, sizeof(DirectionalLight));
		buffer.WriteMapped(Points.data(), PointsSize(), sizeof(DirectionalLight));
		buffer.UnmapMemory();
	}

}