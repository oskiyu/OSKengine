#include "LightsUBO.h"

#include <ext\matrix_clip_space.hpp>

namespace OSK {

	void LightUBO::UpdateBuffer(VkDevice logicalDevice, GpuDataBuffer& buffer) {
		buffer.MapMemory();
		buffer.WriteMapped(&directional, sizeof(DirectionalLight));
		buffer.WriteMapped(points.data(), PointsSize(), sizeof(DirectionalLight));
		buffer.UnmapMemory();
	}

}