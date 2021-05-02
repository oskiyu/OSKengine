#pragma once

#include "OSKmacros.h"
#include "ContentManager.h"
#include "RenderTarget.h"
#include "LightsUBO.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#undef near
#undef far

namespace OSK {

	class RenderAPI;

	struct PointLightShadowUBO {

		void Update(float near, float far, const Vector3f& pos) {
			glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, near, far);

			Matrices[0] = proj * glm::lookAt(pos.ToGLM(), pos.ToGLM() + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
			Matrices[1] = proj * glm::lookAt(pos.ToGLM(), pos.ToGLM() + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
			Matrices[2] = proj * glm::lookAt(pos.ToGLM(), pos.ToGLM() + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
			Matrices[3] = proj * glm::lookAt(pos.ToGLM(), pos.ToGLM() + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
			Matrices[4] = proj * glm::lookAt(pos.ToGLM(), pos.ToGLM() + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
			Matrices[5] = proj * glm::lookAt(pos.ToGLM(), pos.ToGLM() + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
		}

		std::array<glm::mat4, 6> Matrices;

	};
	struct PointLightShadowPushConst {
		glm::mat4 model;
		uint32_t face;
	};

	constexpr auto CUBE_SHADOW_MAP_FORMAT = VK_FORMAT_D32_SFLOAT;
	constexpr auto CUBE_SHADOW_MAP_FILTER = VK_FILTER_NEAREST;

	constexpr auto CUBE_SHADOW_MAP_DEPTH_BIAS_CONSTANT = 1.25f;
	constexpr auto CUBE_SHADOW_MAP_DEPTH_BIAS_SLOPE = 1.75f;

	class OSKAPI_CALL CubeShadowMap {

	public:

		CubeShadowMap(RenderAPI* renderer, ContentManager* content);
		~CubeShadowMap();

		void Create(const Vector2ui& size);
		void Clear();
		void Update();

		PointLightShadowUBO ShadowUBO;

		DescriptorLayout* PointShadowDescriptorLayout = nullptr;
		GraphicsPipeline* ShadowsPipeline = nullptr;
		std::array<RenderTarget*, 6> CubemapTargets;

		std::vector<GPUDataBuffer> UBOs;

		float DepthRangeNear = 0.0f;
		float DepthRangeFar = 100.0f;
		float Density = 20;

		PointLight* Light = nullptr;
		RenderAPI* renderer = nullptr;
		Vector2ui Size;

		uint32_t Face = 0;

	private:

		void CreateDescSets(uint32_t maxSets);
		void CreateBuffers();
		void UpdateBuffers();

		void CreateRenderpass();
		void CreateFramebuffers();
		void CreateGraphicsPipeline();

		ContentManager* Content = nullptr;

	};

}