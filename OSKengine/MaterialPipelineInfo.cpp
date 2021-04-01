#include "MaterialPipelineInfo.h"


VkPolygonMode OSK::GetVkPolygonMode(PolygonMode mode) {
	switch (mode) {
	case OSK::PolygonMode::FILL:
		return VK_POLYGON_MODE_FILL;
		break;
	case OSK::PolygonMode::LINE:
		return VK_POLYGON_MODE_LINE;
		break;
	default:
		return VK_POLYGON_MODE_FILL;
		break;
	}
}

VkCullModeFlagBits OSK::GetVkCullMode(PolygonCullMode mode) {
	switch (mode)
	{
	case OSK::PolygonCullMode::FRONT:
		return VK_CULL_MODE_FRONT_BIT;
		break;
	case OSK::PolygonCullMode::BACK:
		return VK_CULL_MODE_BACK_BIT;
		break;
	case OSK::PolygonCullMode::NONE:
		return VK_CULL_MODE_NONE;
		break;
	default:
		return VK_CULL_MODE_NONE;
		break;
	}
}

VkFrontFace OSK::GetVkPolygonFrontFace(PolygonFrontFaceType mode) {
	switch (mode)
	{
	case OSK::PolygonFrontFaceType::CLOCKWISE:
		return VK_FRONT_FACE_CLOCKWISE;
		break;
	case OSK::PolygonFrontFaceType::COUNTERCLOCKWISE:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		break;
	default:
		return VK_FRONT_FACE_CLOCKWISE;
		break;
	}
}