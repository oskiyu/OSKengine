#pragma once

#include <vector>

#include "Model.h"
#include "AnimatedModel.h"
#include "Skybox.h"
#include "ContentManager.h"
#include "GraphicsPipeline.h"
#include "LightsUBO.h"
#include "Terrain.h"

namespace OSK {

	class VulkanRenderer;

	class RenderizableScene {

		friend class VulkanRenderer;

	public:

		RenderizableScene(VulkanRenderer* renderer);

		~RenderizableScene();

		void AddModel(Model* model);
		void AddAnimatedModel(AnimatedModel* model);

		void LoadSkybox(const std::string& path);

		void SetGraphicsPipeline(GraphicsPipeline* pipeline);

		void Draw(VkCommandBuffer cmdBuffer, const uint32_t& iteration);

		void CreateTerrain(const uint32_t n_quadX, const uint32_t& n_quadY, const Vector2f& quadSize, const float_t& baseHeight = 0.0f);
		void LoadTerrain(const std::string& path, const Vector2f& quadSize, const float_t& maxHeight);
		void SetTerrain(Terrain* terreno);

		LightUBO Lights = {};

		Terrain* terreno;

	protected:

		GraphicsPipeline* CurrentGraphicsPipeline = nullptr;

		GraphicsPipeline* PhongPipeline = nullptr;
		GraphicsPipeline* PBR_Pipeline = nullptr;
		GraphicsPipeline* SkyboxPipeline = nullptr;

		ContentManager* Content = nullptr;

		std::vector<VulkanBuffer> LightsUniformBuffers;

	private:

		Skybox skybox;

		std::vector<Model*> Models = {};
		std::vector<AnimatedModel*> AnimatedModels = {};

		VulkanRenderer* renderer;

		bool isPropetaryOfTerrain = true;

	};

}
