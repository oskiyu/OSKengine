#pragma once

#include <vulkan/vulkan.h>
#include <cmath>

#include "VulkanImage.h"
#include "Renderpass.h"
#include "GraphicsPipeline.h"
#include "RenderTarget.h"
#include "LightsUBO.h"

namespace OSK {

	struct DirLightShadowUBO {
		alignas(16) glm::mat4 DirLightMat = glm::mat4(1.0f);
	};

	constexpr auto SHADOW_MAP_FORMAT = VK_FORMAT_D32_SFLOAT;
	constexpr auto SHADOW_MAP_FILTER = VK_FILTER_NEAREST;

	constexpr auto SHADOW_MAP_DEPTH_BIAS_CONSTANT = 1.25f;
	constexpr auto SHADOW_MAP_DEPTH_BIAS_SLOPE = 1.75f;

	class OSKAPI_CALL ShadowMap {

		friend class RenderizableScene;

	public:

		ShadowMap(RenderAPI* renderer, ContentManager* content);

		~ShadowMap();

		void Create(const Vector2ui& size);

		void Clear();

		void CreateDescriptorSet(ModelTexture* texture);

		DirLightShadowUBO DirShadowsUBO = {};

		Vector2ui Size;

		RenderTarget* DirShadows = nullptr;
		DescriptorLayout* DirShadowDescriptorLayout = nullptr;
		GraphicsPipeline* ShadowsPipeline = nullptr;

		std::vector<VulkanBuffer> DirShadowsUniformBuffers;

		float DepthRangeNear = 0.0f;
		float DepthRangeFar = 100.0f;

		float Density = 20;

		LightUBO* Lights = nullptr;
		
		RenderAPI* renderer = nullptr;

	private:

		void CreateDescSets(uint32_t maxSets);
		void CreateBuffers();
		void UpdateBuffers();
		void Update();

		void CreateRenderpass();
		void CreateFramebuffers();
		void CreateGraphicsPipeline();

		ContentManager* Content = nullptr;

	};

}
