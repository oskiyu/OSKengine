#include "RenderizableScene.h"

#include "VulkanRenderer.h"
#include "DescriptorSet.h"

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
		if (terreno != nullptr)
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

	void RenderizableScene::CreatePhongDescriptorSet(ModelTexture* texture, VkSampler albedoSampler, VkSampler specularSampler) const {
		texture->PhongDescriptorSet = renderer->CreateNewDescriptorSet();
		texture->PhongDescriptorSet->SetDescriptorLayout(renderer->PhongDescriptorLayout);
		texture->PhongDescriptorSet->AddUniformBuffers(renderer->UniformBuffers, 0, sizeof(UBO));
		texture->PhongDescriptorSet->AddImage(&texture->Albedo, albedoSampler, 1);
		texture->PhongDescriptorSet->AddUniformBuffers(LightsUniformBuffers, 2, Lights.Size());
		texture->PhongDescriptorSet->AddImage(&texture->Specular, specularSampler, 3);
		texture->PhongDescriptorSet->Create();
	}

	void RenderizableScene::LoadHeightmap(const std::string& path, const Vector2f& quadSize, const float_t& maxHeight) {
		if (terreno != nullptr) {
			delete terreno;
			terreno = nullptr;
		}
		terreno = new Terrain(Content);

		terreno->CreateMesh(path, quadSize, maxHeight);
	}

	void RenderizableScene::Draw(VkCommandBuffer cmdBuffer, const uint32_t& iteration) {
		SkyboxPipeline->Bind(cmdBuffer);
		skybox.Bind(cmdBuffer, SkyboxPipeline, iteration);
		skybox.Draw(cmdBuffer);

		CurrentGraphicsPipeline->Bind(cmdBuffer);

		for (const auto& i : Models) {
			i->Bind(cmdBuffer);

			if (i->texture != nullptr)
				i->texture->PhongDescriptorSet->Bind(cmdBuffer, CurrentGraphicsPipeline, iteration);
			else
				CurrentGraphicsPipeline->DefaultTexture->PhongDescriptorSet->Bind(cmdBuffer, CurrentGraphicsPipeline, iteration);

			PushConst3D pushConst = i->GetPushConst();
			vkCmdPushConstants(cmdBuffer, CurrentGraphicsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			i->Draw(cmdBuffer);
		}

		if (terreno != nullptr && terreno->terrainModel != nullptr) {
			CurrentGraphicsPipeline->DefaultTexture->PhongDescriptorSet->Bind(cmdBuffer, CurrentGraphicsPipeline, iteration);
			terreno->terrainModel->Bind(cmdBuffer);
			PushConst3D pushConst{ glm::mat4(1.0f) };
			vkCmdPushConstants(cmdBuffer, CurrentGraphicsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			terreno->terrainModel->Draw(cmdBuffer);
		}

	}

}