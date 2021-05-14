#include "RenderizableScene.h"

#include "VulkanRenderer.h"
#include "DescriptorSet.h"

namespace OSK {

	RenderizableScene::RenderizableScene(RenderAPI* renderer) {
		this->renderer = renderer;
		
		content = new ContentManager(this->renderer);
		shadowMap = new ShadowMap(renderer, content.GetPointer());
		shadowMap->Create({ 1024, 1024 });

		SetupLightsUBO();
		InitLightsBuffers();
		UpdateLightsBuffers();

		for (uint32_t i = 0; i < lights.points.size(); i++) {
			/*CubeShadowMap* map = new CubeShadowMap(renderer, Content);
			map->Light = &Lights.Points[i];
			map->Create({ 512 });

			cubeShadowMaps.push_back(map);*/
		}
	}

	RenderizableScene::~RenderizableScene() {

	}

	void RenderizableScene::SetupLightsUBO() {
		lights.points.resize(16);
		lights.points[0].color = OSK::Color(1.0f, 1.0f, 1.0f).ToGLM();
		lights.points[0].information.x = 2.0f;
		lights.points[0].information.y = 200.0f;
		lights.points[0].position = { 5, 5, 5 };
		
		const Color color = Color::RED();
		lights.directional = DirectionalLight{ OSK::Vector3(-2, 4, 1), color, 1 };

		shadowMap->lights = &lights;
	}

	void RenderizableScene::InitLightsBuffers() {
		VkDeviceSize size = lights.Size();
		lightsUniformBuffers.resize(renderer->swapchainImages.size());
		for (uint32_t i = 0; i < lightsUniformBuffers.size(); i++) {
			lightsUniformBuffers[i] = renderer->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			lightsUniformBuffers[i].Allocate(size);
		}

		bonesUbos.resize(renderer->swapchainImages.size());
		for (uint32_t i = 0; i < bonesUbos.size(); i++) {
			renderer->CreateDynamicUBO(bonesUbos[i], sizeof(AnimUBO), 64);

			AnimUBO ubo{};
			bonesUbos[i].Write(&ubo, sizeof(AnimUBO));
		}
	}

	void RenderizableScene::UpdateLightsBuffers() {
		for (auto& i : lightsUniformBuffers)
			lights.UpdateBuffer(renderer->logicalDevice, i);

		shadowMap->Update();
		shadowMap->UpdateBuffers();

		/*for (uint32_t i = 0; i < cubeShadowMaps.size(); i++) {
			cubeShadowMaps[i]->Light = &lights.points[i];
			cubeShadowMaps[i]->Update();
		}*/
	}

	void RenderizableScene::AddModel(Model* model) {
		models.push_back(model);
	}

	void RenderizableScene::AddAnimatedModel(AnimatedModel* model) {
		AddModel(model);
		animatedModels.push_back(model);
	}

	void RenderizableScene::LoadSkybox(const std::string& path) {
		content->LoadSkybox(skybox, path);
	}

	void RenderizableScene::LoadHeightmap(const std::string& path, const Vector2f& quadSize, float maxHeight) {
		terreno = new Terrain(content.GetPointer());

		terreno->CreateMesh(path, quadSize, maxHeight);

		terreno->material = renderer->GetMaterialSystem()->GetMaterial(renderer->defaultMaterial3D_Name)->CreateInstance();
		terreno->material->SetBuffer("Camera", renderer->GetUniformBuffers());
		terreno->material->SetDynamicBuffer("Animation", bonesUbos);
		terreno->material->SetBuffer("Model", shadowMap->dirShadowsUniformBuffers);
		terreno->material->SetTexture("Albedo", content->DefaultTexture);
		terreno->material->SetBuffer("Lights", lightsUniformBuffers);
		terreno->material->SetTexture("Specular", content->DefaultTexture);
		terreno->material->SetTexture("Shadows", shadowMap->dirShadows->renderedSprite.texture);
		terreno->material->FlushUpdate();
	}

	void RenderizableScene::DrawShadows(VkCommandBuffer cmdBuffer, uint32_t iteration) {
		PrepareDrawShadows(cmdBuffer, iteration);

		for (const auto& i : models) {
			DrawShadows(i, cmdBuffer, iteration);
		}

		EndDrawShadows(cmdBuffer, iteration);
	}

	/*void RenderizableScene::DrawPointShadows(VkCommandBuffer cmdBuffer, uint32_t iteration, CubeShadowMap* map) {
		/*for (uint32_t face = 0; face < 6; face++) {
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
	}*/

	void RenderizableScene::Draw(VkCommandBuffer cmdBuffer, uint32_t iteration) {
		PrepareDraw(cmdBuffer, iteration);

		for (const auto& i : models) {
			Draw(i, cmdBuffer, iteration);
		}

		EndDraw(cmdBuffer, iteration);
	}

	//SYSTEM

	void RenderizableScene::PrepareDrawShadows(VkCommandBuffer cmdBuffer, uint32_t i) {
		VULKAN::VulkanImageGen::TransitionImageLayout(&shadowMap->dirShadows->renderedSprite.texture->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1, &cmdBuffer);

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = shadowMap->dirShadows->vulkanRenderpass->vulkanRenderpass;
		renderPassInfo.framebuffer = shadowMap->dirShadows->targetFramebuffers[i]->framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };

		renderPassInfo.renderArea.extent = { shadowMap->dirShadows->GetSize().X, shadowMap->dirShadows->GetSize().Y };
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0] = { 1.0f, 1.0f, 1.0f, 1.0f }; //Color.
		clearValues[1] = { 1.0f, 0.0f }; //Depth.
		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		renderer->SetViewport(cmdBuffer, 0, 0, shadowMap->GetImageSize().X, shadowMap->GetImageSize().Y);

		renderer->GetMaterialSystem()->GetMaterial(renderer->defaultShadowsMaterial_Name)->GetGraphicsPipeline(shadowMap->dirShadows->vulkanRenderpass)->Bind(cmdBuffer);
	}
	void RenderizableScene::DrawShadows(Model* model, VkCommandBuffer cmdBuffer, uint32_t i) {
		if (!model->shadowMaterial.HasValue() || !model->shadowMaterial->HasBeenSet())
			return;

		model->Bind(cmdBuffer);

		model->shadowMaterial->GetDescriptorSet()->Bind(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(renderer->defaultShadowsMaterial_Name)->GetGraphicsPipeline(shadowMap->dirShadows->vulkanRenderpass), i, model->animationBufferOffset, bonesUbos[0].alignment);

		PushConst3D pushConst = model->GetPushConst();
		vkCmdPushConstants(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(renderer->defaultShadowsMaterial_Name)->GetGraphicsPipeline(shadowMap->dirShadows->vulkanRenderpass)->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
		model->Draw(cmdBuffer);
	}
	void RenderizableScene::EndDrawShadows(VkCommandBuffer cmdBuffer, uint32_t i) {
		if (terreno.HasValue() && terreno->terrainModel != nullptr && terreno->shadowMaterial.HasValue() && terreno->shadowMaterial->HasBeenSet()) {
			terreno->shadowMaterial->GetDescriptorSet()->Bind(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(renderer->defaultShadowsMaterial_Name)->GetGraphicsPipeline(shadowMap->dirShadows->vulkanRenderpass), i, 0, bonesUbos[0].alignment);
			terreno->terrainModel->Bind(cmdBuffer);
			PushConst3D pushConst{ glm::mat4(1.0f) };
			vkCmdPushConstants(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(renderer->defaultShadowsMaterial_Name)->GetGraphicsPipeline(shadowMap->dirShadows->vulkanRenderpass)->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			terreno->terrainModel->Draw(cmdBuffer);
		}

		vkCmdEndRenderPass(cmdBuffer);
		VULKAN::VulkanImageGen::TransitionImageLayout(&shadowMap->dirShadows->renderedSprite.texture->image, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1, &cmdBuffer);
	}

	void RenderizableScene::PrepareDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
		GraphicsPipeline* pipeline = renderer->GetMaterialSystem()->GetMaterial(renderer->defaultSkyboxMaterial_Name)->GetGraphicsPipeline(targetRenderpass);
		pipeline->Bind(cmdBuffer);

		skybox.Bind(cmdBuffer, pipeline, i);
		skybox.Draw(cmdBuffer);
		
		currentGraphicsPipeline = renderer->GetMaterialSystem()->GetMaterial(renderer->defaultMaterial3D_Name)->GetGraphicsPipeline(targetRenderpass);
		currentGraphicsPipeline->Bind(cmdBuffer);

		UpdateLightsBuffers();
	}
	void RenderizableScene::Draw(Model* model, VkCommandBuffer cmdBuffer, uint32_t i) {
		if (!model->material->HasBeenSet())
			return;

		model->Bind(cmdBuffer);

		model->material->GetDescriptorSet()->Bind(cmdBuffer, currentGraphicsPipeline, i, model->animationBufferOffset, bonesUbos[0].alignment);
		
		PushConst3D pushConst = model->GetPushConst();
		vkCmdPushConstants(cmdBuffer, currentGraphicsPipeline->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
		model->Draw(cmdBuffer);
	}
	void RenderizableScene::EndDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
		if (terreno.HasValue() && terreno->terrainModel != nullptr) {
			terreno->material->GetDescriptorSet()->Bind(cmdBuffer, currentGraphicsPipeline, i, 0, bonesUbos[0].alignment);
			terreno->terrainModel->Bind(cmdBuffer);
			PushConst3D pushConst{ glm::mat4(1.0f) };
			vkCmdPushConstants(cmdBuffer, currentGraphicsPipeline->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			terreno->terrainModel->Draw(cmdBuffer);
		}
	}

}
