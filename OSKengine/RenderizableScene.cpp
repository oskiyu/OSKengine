#include "RenderizableScene.h"

#include "VulkanRenderer.h"

namespace OSK {

	RenderizableScene::RenderizableScene(VulkanRenderer* renderer) {
		this->renderer = renderer;

		PhongPipeline = renderer->GraphicsPipeline3D;
		SkyboxPipeline = renderer->SkyboxGraphicsPipeline;

		CurrentGraphicsPipeline = PhongPipeline;

		Content = new ContentManager(this->renderer);
	}

	RenderizableScene::~RenderizableScene() {
		delete Content;
		if (isPropetaryOfTerrain)
			delete terreno;
	}

	void RenderizableScene::AddModel(Model* model) {
		Models.push_back(model);
	}

	void RenderizableScene::AddAnimatedModel(AnimatedModel* model) {
		AddModel(model);
		AnimatedModels.push_back(model);
	}

	void RenderizableScene::LoadSkybox(const std::string& path) {
		Content->LoadSkybox(skybox, path);
	}

	void RenderizableScene::SetGraphicsPipeline(GraphicsPipeline* pipeline) {
		CurrentGraphicsPipeline = pipeline;
	}

	void RenderizableScene::CreateTerrain(const uint32_t n_quadX, const uint32_t& n_quadY, const Vector2f& quadSize, const float_t& baseHeight) {
		if (terreno == nullptr) {
			terreno = new Terrain(Content);
			isPropetaryOfTerrain = true;
		}

	//	terreno->Width = n_quadX + 1;
	//	terreno->Height = n_quadY + 1;
	//	terreno->Size = quadSize;
	//	terreno->BaseHeight = baseHeight;

		//terreno->CreateMesh();
	}

	void RenderizableScene::LoadTerrain(const std::string& path, const Vector2f& quadSize, const float_t& maxHeight) {
		terreno = new Terrain(Content);

		terreno->CreateMesh(path, quadSize, maxHeight);
	}

	void RenderizableScene::SetTerrain(Terrain* terreno) {
		this->terreno = terreno;
		isPropetaryOfTerrain = false;
	}

	void RenderizableScene::Draw(VkCommandBuffer cmdBuffer, const uint32_t& iteration) {
		SkyboxPipeline->Bind(cmdBuffer);
		skybox.Bind(cmdBuffer, SkyboxPipeline, iteration);
		skybox.Draw(cmdBuffer);

		CurrentGraphicsPipeline->Bind(cmdBuffer);

		for (const auto& i : Models) {
			i->Bind(cmdBuffer);
			i->texture->PhongDescriptorSet->Bind(cmdBuffer, CurrentGraphicsPipeline, iteration);
			PushConst3D pushConst = i->GetPushConst();
			vkCmdPushConstants(cmdBuffer, CurrentGraphicsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			i->Draw(cmdBuffer);
		}

		if (terreno != nullptr && terreno->terrainModel != nullptr) {
			terreno->terrainModel->Bind(cmdBuffer);
			PushConst3D pushConst{ glm::mat4(1.0f) };
			vkCmdPushConstants(cmdBuffer, CurrentGraphicsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			terreno->terrainModel->Draw(cmdBuffer);
		}

	}

}