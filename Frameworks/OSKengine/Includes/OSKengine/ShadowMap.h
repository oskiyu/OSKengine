#pragma once

#include <vulkan/vulkan.h>
#include <cmath>

#include "VulkanImage.h"
#include "Renderpass.h"
#include "GraphicsPipeline.h"

namespace OSK {

	constexpr auto SHADOW_MAP_FORMAT = VK_FORMAT_D32_SFLOAT;
	constexpr auto SHADOW_MAP_FILTER = VK_FILTER_LINEAR;

	constexpr auto SHADOW_MAP_DEPTH_BIAS_CONSTANT = 1.25f;
	constexpr auto SHADOW_MAP_DEPTH_BIAS_SLOPE = 1.75f;

	struct OffscreenPass {
		int32_t SizeX;
		int32_t SizeY;

		VkFramebuffer Framebuffer;

		VULKAN::VulkanImage Image;
		VULKAN::Renderpass Renderpass;

		VkSampler DepthSampler;
		VkDescriptorImageInfo ImageInfo;
		GraphicsPipeline* Pipeline;
	};

	struct ShadowMap {

		ShadowMap() {};

		Vector2ui ImageSize;

		VULKAN::VulkanImage Image;
		VULKAN::Renderpass* Renderpass;

		/*float_t DepthRangeNear = 1.0f;
		float_t DepthRangeFar = 96.0f;

		const float_t LightFOV = 45.0f;

		OffscreenPass Pass;*/

	};

}
