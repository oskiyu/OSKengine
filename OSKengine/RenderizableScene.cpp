#include "RenderizableScene.h"

#include "VulkanRenderer.h"
#include "DescriptorSet.h"

namespace OSK {

	RenderizableScene::RenderizableScene(RenderAPI* renderer) {
		this->renderer = renderer;

		SkyboxPipeline = renderer->SkyboxGraphicsPipeline;
		SetGraphicsPipeline();

		CreateDescriptorLayout();
		SetupLightsUBO();
		InitLightsBuffers();
		UpdateLightsBuffers();

		Content = new ContentManager(this->renderer);

		DefaultTexture = Content->LoadModelTexture(ContentManager::DEFAULT_TEXTURE_PATH);
		CreateDescriptorSet(DefaultTexture);
	}

	RenderizableScene::~RenderizableScene() {
		delete Content;
		if (Terreno != nullptr)
			delete Terreno;
		delete PhongDescriptorLayout;
	}

	void RenderizableScene::CreateDescriptorLayout() {
		PhongDescriptorLayout = renderer->CreateNewDescriptorLayout();
		PhongDescriptorLayout->AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		PhongDescriptorLayout->AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		PhongDescriptorLayout->AddBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		PhongDescriptorLayout->AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		PhongDescriptorLayout->Create(renderer->Settings.MaxTextures);
	}

	void RenderizableScene::SetupLightsUBO() {
		Lights.Points.resize(16);
		Lights.Points.push_back({});
		Lights.Points[0].infos.y = 100;
		Lights.Points[0].Color = OSK::Color(0.0f, 1.0f, 1.0f).ToGLM();
		Lights.Points[0].infos.x = 2.0f;
		Lights.Points[0].Position = { 5, 5, 5 };
		
		Lights.Directional = DirectionalLight{ OSK::Vector3(-1, 1, 0), OSK::Color::RED(), 1 };
	}

	void RenderizableScene::InitLightsBuffers() {
		VkDeviceSize size = Lights.Size();
		LightsUniformBuffers.resize(renderer->SwapchainImages.size());

		for (uint32_t i = 0; i < LightsUniformBuffers.size(); i++)
			renderer->CreateBuffer(LightsUniformBuffers[i], size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

	void RenderizableScene::CreateDescriptorSet(ModelTexture* texture) const {
		if (texture->PhongDescriptorSet != nullptr)
			delete texture->PhongDescriptorSet;

		texture->PhongDescriptorSet = renderer->CreateNewDescriptorSet();
		texture->PhongDescriptorSet->SetDescriptorLayout(PhongDescriptorLayout);
		texture->PhongDescriptorSet->AddUniformBuffers(renderer->UniformBuffers, 0, sizeof(UBO));
		texture->PhongDescriptorSet->AddImage(&texture->Albedo, texture->Albedo.Sampler, 1);
		texture->PhongDescriptorSet->AddUniformBuffers(LightsUniformBuffers, 2, Lights.Size());
		texture->PhongDescriptorSet->AddImage(&texture->Specular, texture->Specular.Sampler, 3);
		texture->PhongDescriptorSet->Create();
	}

	void RenderizableScene::UpdateLightsBuffers() {
		for (auto& i : LightsUniformBuffers)
			Lights.UpdateBuffer(renderer->LogicalDevice, i);
	}

	void RenderizableScene::AddModel(Model* model) {
		Models.push_back(model);
		CreateDescriptorSet(model->texture);
	}

	void RenderizableScene::AddAnimatedModel(AnimatedModel* model) {
		AddModel(model);
		AnimatedModels.push_back(model);
	}

	void RenderizableScene::LoadSkybox(const std::string& path) {
		Content->LoadSkybox(skybox, path);
	}

	void RenderizableScene::SetGraphicsPipeline() {
		CurrentGraphicsPipeline = renderer->CreateNewPhongPipeline(renderer->Settings.VertexShaderPath3D, renderer->Settings.FragmentShaderPath3D);
	}

	void RenderizableScene::RecreateGraphicsPipeline() {
		delete CurrentGraphicsPipeline;
		SetGraphicsPipeline();
		SkyboxPipeline = renderer->SkyboxGraphicsPipeline;
	}

	void RenderizableScene::LoadHeightmap(const std::string& path, const Vector2f& quadSize, const float_t& maxHeight) {
		if (Terreno != nullptr) {
			delete Terreno;
			Terreno = nullptr;
		}
		Terreno = new Terrain(Content);

		Terreno->CreateMesh(path, quadSize, maxHeight);
	}

	void RenderizableScene::Draw(VkCommandBuffer cmdBuffer, const uint32_t& iteration) {
		SkyboxPipeline->Bind(cmdBuffer);
		skybox.Bind(cmdBuffer, SkyboxPipeline, iteration);
		skybox.Draw(cmdBuffer);

		CurrentGraphicsPipeline->Bind(cmdBuffer);

		UpdateLightsBuffers();

		for (const auto& i : Models) {
			i->Bind(cmdBuffer);

			if (i->texture != nullptr)
				i->texture->PhongDescriptorSet->Bind(cmdBuffer, CurrentGraphicsPipeline, iteration);
			else
				DefaultTexture->PhongDescriptorSet->Bind(cmdBuffer, CurrentGraphicsPipeline, iteration);

			PushConst3D pushConst = i->GetPushConst();
			vkCmdPushConstants(cmdBuffer, CurrentGraphicsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			i->Draw(cmdBuffer);
		}

		if (Terreno != nullptr && Terreno->terrainModel != nullptr) {
			DefaultTexture->PhongDescriptorSet->Bind(cmdBuffer, CurrentGraphicsPipeline, iteration);
			Terreno->terrainModel->Bind(cmdBuffer);
			PushConst3D pushConst{ glm::mat4(1.0f) };
			vkCmdPushConstants(cmdBuffer, CurrentGraphicsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			Terreno->terrainModel->Draw(cmdBuffer);
		}

	}

}