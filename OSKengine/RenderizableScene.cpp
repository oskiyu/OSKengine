#include "RenderizableScene.h"

#include "VulkanRenderer.h"
#include "DescriptorSet.h"

namespace OSK {

	RenderizableScene::RenderizableScene(RenderAPI* renderer, uint32_t maxInitEntities) {
		this->renderer = renderer;
		
		SkyboxPipeline = renderer->DefaultSkyboxGraphicsPipeline;
		SetGraphicsPipeline();

		Content = new ContentManager(this->renderer);
		shadowMap = new ShadowMap(renderer, Content);
		shadowMap->Create({ 1024, 1024 });

		CreateDescriptorLayout(maxInitEntities);
		SetupLightsUBO();
		InitLightsBuffers();
		UpdateLightsBuffers();

		for (uint32_t i = 0; i < Lights.Points.size(); i++) {
			CubeShadowMap* map = new CubeShadowMap(renderer, Content);
			map->Light = &Lights.Points[i];
			map->Create({ 512 });

			cubeShadowMaps.push_back(map);
		}

		DefaultTexture = Content->LoadModelTexture(ContentManager::DEFAULT_TEXTURE_PATH);

		CreateDescriptorSet(DefaultTexture);
	}

	RenderizableScene::~RenderizableScene() {
		delete Content;
		if (Terreno != nullptr)
			delete Terreno;
		delete PhongDescriptorLayout;

		delete shadowMap;
	}

	void RenderizableScene::CreateDescriptorLayout(uint32_t maxSets) {
		PhongDescriptorLayout = renderer->CreateNewDescriptorLayout();
		PhongDescriptorLayout->AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		PhongDescriptorLayout->AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		PhongDescriptorLayout->AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		PhongDescriptorLayout->AddBinding(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		PhongDescriptorLayout->AddBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		PhongDescriptorLayout->AddBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		PhongDescriptorLayout->Create(maxSets);
	}

	void RenderizableScene::SetupLightsUBO() {
		Lights.Points.resize(16);
		Lights.Points[0].Color = OSK::Color(1.0f, 1.0f, 1.0f).ToGLM();
		Lights.Points[0].infos.x = 2.0f;
		Lights.Points[0].infos.y = 200.0f;
		Lights.Points[0].Position = { 5, 5, 5 };
		
		const Color color = Color::RED();
		Lights.Directional = DirectionalLight{ OSK::Vector3(-2, 4, 1), color, 1 };

		shadowMap->Lights = &Lights;
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
		texture->PhongDescriptorSet->AddUniformBuffers(shadowMap->DirShadowsUniformBuffers, 1, sizeof(DirLightShadowUBO));
		texture->PhongDescriptorSet->AddImage(&texture->Albedo, texture->Albedo.Sampler, 2);
		texture->PhongDescriptorSet->AddUniformBuffers(LightsUniformBuffers, 3, Lights.Size());
		texture->PhongDescriptorSet->AddImage(&texture->Specular, texture->Specular.Sampler, 4);
		texture->PhongDescriptorSet->AddImage(&shadowMap->DirShadows->RenderedSprite.texture->Albedo, shadowMap->DirShadows->RenderedSprite.texture->Albedo.Sampler, 5);
		texture->PhongDescriptorSet->Create();

		shadowMap->CreateDescriptorSet(texture);
		cubeShadowMaps[0]->CreateDescriptorSet(texture);
	}

	void RenderizableScene::UpdateLightsBuffers() {
		for (auto& i : LightsUniformBuffers)
			Lights.UpdateBuffer(renderer->LogicalDevice, i);

		shadowMap->Update();
		shadowMap->UpdateBuffers();

		for (uint32_t i = 0; i < cubeShadowMaps.size(); i++) {
			cubeShadowMaps[i]->Light = &Lights.Points[i];
			cubeShadowMaps[i]->Update();
		}
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
		SkyboxPipeline = renderer->DefaultSkyboxGraphicsPipeline;
	}

	void RenderizableScene::LoadHeightmap(const std::string& path, const Vector2f& quadSize, float maxHeight) {
		if (Terreno != nullptr) {
			delete Terreno;
			Terreno = nullptr;
		}
		Terreno = new Terrain(Content);

		Terreno->CreateMesh(path, quadSize, maxHeight);
	}

	void RenderizableScene::DrawShadows(VkCommandBuffer cmdBuffer, uint32_t iteration) {
		PrepareDrawShadows(cmdBuffer, iteration);

		for (const auto& i : Models) {
			DrawShadows(i, cmdBuffer, iteration);
		}

		EndDrawShadows(cmdBuffer, iteration);
	}

	void RenderizableScene::DrawPointShadows(VkCommandBuffer cmdBuffer, uint32_t iteration, CubeShadowMap* map) {
		for (uint32_t face = 0; face < 6; face++) {
			VULKAN::VulkanImageGen::TransitionImageLayout(&map->CubemapTargets[face]->RenderedSprite.texture->Albedo, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1, &cmdBuffer);

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = map->CubemapTargets[face]->VRenderpass->VulkanRenderpass;
			renderPassInfo.framebuffer = map->CubemapTargets[face]->TargetFramebuffers[iteration]->framebuffer;
			renderPassInfo.renderArea.offset = { 0, 0 };

			renderPassInfo.renderArea.extent = { map->CubemapTargets[face]->Size.X, map->CubemapTargets[face]->Size.Y };
			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0] = { 1.0f, 1.0f, 1.0f, 1.0f }; //Color.
			clearValues[1] = { 1.0f, 0.0f }; //Depth.
			renderPassInfo.clearValueCount = clearValues.size();
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			map->ShadowsPipeline->Bind(cmdBuffer);

			for (const auto& i : Models) {
				i->Bind(cmdBuffer);

				if (i->texture != nullptr)
					i->texture->PointShadowsDescriptorSet->Bind(cmdBuffer, map->ShadowsPipeline, iteration);
				else
					DefaultTexture->PointShadowsDescriptorSet->Bind(cmdBuffer, map->ShadowsPipeline, iteration);

				PointLightShadowPushConst pushConst = { i->GetPushConst().model, face };
				vkCmdPushConstants(cmdBuffer, map->ShadowsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PointLightShadowPushConst), &pushConst);
				i->Draw(cmdBuffer);
			}

			if (Terreno != nullptr && Terreno->terrainModel != nullptr) {
				DefaultTexture->PointShadowsDescriptorSet->Bind(cmdBuffer, map->ShadowsPipeline, iteration);
				Terreno->terrainModel->Bind(cmdBuffer);
				PushConst3D pushConst{ glm::mat4(1.0f) };
				vkCmdPushConstants(cmdBuffer, map->ShadowsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
				Terreno->terrainModel->Draw(cmdBuffer);
			}

			vkCmdEndRenderPass(cmdBuffer);
			VULKAN::VulkanImageGen::TransitionImageLayout(&map->CubemapTargets[face]->RenderedSprite.texture->Albedo, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1, &cmdBuffer);
		}
	}

	void RenderizableScene::Draw(VkCommandBuffer cmdBuffer, uint32_t iteration) {
		PrepareDraw(cmdBuffer, iteration);

		for (const auto& i : Models) {
			Draw(i, cmdBuffer, iteration);
		}

		EndDraw(cmdBuffer, iteration);
	}

	//SYSTEM

	void RenderizableScene::PrepareDrawShadows(VkCommandBuffer cmdBuffer, uint32_t i) {
		VULKAN::VulkanImageGen::TransitionImageLayout(&shadowMap->DirShadows->RenderedSprite.texture->Albedo, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1, &cmdBuffer);

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = shadowMap->DirShadows->VRenderpass->VulkanRenderpass;
		renderPassInfo.framebuffer = shadowMap->DirShadows->TargetFramebuffers[i]->framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };

		renderPassInfo.renderArea.extent = { shadowMap->DirShadows->Size.X, shadowMap->DirShadows->Size.Y };
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0] = { 1.0f, 1.0f, 1.0f, 1.0f }; //Color.
		clearValues[1] = { 1.0f, 0.0f }; //Depth.
		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		shadowMap->ShadowsPipeline->Bind(cmdBuffer);
	}
	void RenderizableScene::DrawShadows(Model* model, VkCommandBuffer cmdBuffer, uint32_t i) {
		model->Bind(cmdBuffer);

		if (model->texture != nullptr)
			model->texture->DirShadowsDescriptorSet->Bind(cmdBuffer, shadowMap->ShadowsPipeline, i);
		else
			DefaultTexture->DirShadowsDescriptorSet->Bind(cmdBuffer, shadowMap->ShadowsPipeline, i);

		PushConst3D pushConst = model->GetPushConst();
		vkCmdPushConstants(cmdBuffer, shadowMap->ShadowsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
		model->Draw(cmdBuffer);
	}
	void RenderizableScene::EndDrawShadows(VkCommandBuffer cmdBuffer, uint32_t i) {
		if (Terreno != nullptr && Terreno->terrainModel != nullptr) {
			DefaultTexture->DirShadowsDescriptorSet->Bind(cmdBuffer, shadowMap->ShadowsPipeline, i);
			Terreno->terrainModel->Bind(cmdBuffer);
			PushConst3D pushConst{ glm::mat4(1.0f) };
			vkCmdPushConstants(cmdBuffer, shadowMap->ShadowsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			Terreno->terrainModel->Draw(cmdBuffer);
		}

		vkCmdEndRenderPass(cmdBuffer);
		VULKAN::VulkanImageGen::TransitionImageLayout(&shadowMap->DirShadows->RenderedSprite.texture->Albedo, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1, &cmdBuffer);
	}

	void RenderizableScene::PrepareDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
		SkyboxPipeline->Bind(cmdBuffer);
		skybox.Bind(cmdBuffer, SkyboxPipeline, i);
		skybox.Draw(cmdBuffer);

		CurrentGraphicsPipeline->Bind(cmdBuffer);

		UpdateLightsBuffers();
	}
	void RenderizableScene::Draw(Model* model, VkCommandBuffer cmdBuffer, uint32_t i) {
		model->Bind(cmdBuffer);

		if (model->texture != nullptr)
			model->texture->PhongDescriptorSet->Bind(cmdBuffer, CurrentGraphicsPipeline, i);
		else
			DefaultTexture->PhongDescriptorSet->Bind(cmdBuffer, CurrentGraphicsPipeline, i);

		PushConst3D pushConst = model->GetPushConst();
		vkCmdPushConstants(cmdBuffer, CurrentGraphicsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
		model->Draw(cmdBuffer);
	}
	void RenderizableScene::EndDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
		if (Terreno != nullptr && Terreno->terrainModel != nullptr) {
			DefaultTexture->PhongDescriptorSet->Bind(cmdBuffer, CurrentGraphicsPipeline, i);
			Terreno->terrainModel->Bind(cmdBuffer);
			PushConst3D pushConst{ glm::mat4(1.0f) };
			vkCmdPushConstants(cmdBuffer, CurrentGraphicsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			Terreno->terrainModel->Draw(cmdBuffer);
		}
	}

}