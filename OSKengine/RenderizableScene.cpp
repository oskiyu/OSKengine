#include "RenderizableScene.h"

#include "VulkanRenderer.h"
#include "DescriptorSet.h"

namespace OSK {

	RenderizableScene::RenderizableScene(RenderAPI* renderer) {
		this->renderer = renderer;
		
		Content = new ContentManager(this->renderer);
		shadowMap = new ShadowMap(renderer, Content.GetPointer());
		shadowMap->Create({ 1024, 1024 });

		SetupLightsUBO();
		InitLightsBuffers();
		UpdateLightsBuffers();

		for (uint32_t i = 0; i < Lights.Points.size(); i++) {
			/*CubeShadowMap* map = new CubeShadowMap(renderer, Content);
			map->Light = &Lights.Points[i];
			map->Create({ 512 });

			cubeShadowMaps.push_back(map);*/
		}
	}

	RenderizableScene::~RenderizableScene() {

	}

	void RenderizableScene::SetupLightsUBO() {
		Lights.Points.resize(16);
		Lights.Points[0].Color = OSK::Color(1.0f, 1.0f, 1.0f).ToGLM();
		Lights.Points[0].Information.x = 2.0f;
		Lights.Points[0].Information.y = 200.0f;
		Lights.Points[0].Position = { 5, 5, 5 };
		
		const Color color = Color::RED();
		Lights.Directional = DirectionalLight{ OSK::Vector3(-2, 4, 1), color, 1 };

		shadowMap->Lights = &Lights;
	}

	void RenderizableScene::InitLightsBuffers() {
		VkDeviceSize size = Lights.Size();
		LightsUniformBuffers.resize(renderer->SwapchainImages.size());
		for (uint32_t i = 0; i < LightsUniformBuffers.size(); i++) {
			LightsUniformBuffers[i] = renderer->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			LightsUniformBuffers[i].Allocate(size);
		}

		BonesUBOs.resize(renderer->SwapchainImages.size());
		for (uint32_t i = 0; i < BonesUBOs.size(); i++) {
			renderer->CreateDynamicUBO(BonesUBOs[i], sizeof(AnimUBO), 64);

			AnimUBO ubo{};
			BonesUBOs[i].Write(&ubo, sizeof(AnimUBO));
		}
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
	}

	void RenderizableScene::AddAnimatedModel(AnimatedModel* model) {
		AddModel(model);
		AnimatedModels.push_back(model);
	}

	void RenderizableScene::LoadSkybox(const std::string& path) {
		Content->LoadSkybox(skybox, path);
	}

	void RenderizableScene::LoadHeightmap(const std::string& path, const Vector2f& quadSize, float maxHeight) {
		Terreno = new Terrain(Content.GetPointer());

		Terreno->CreateMesh(path, quadSize, maxHeight);

		Terreno->TerrainMaterial = renderer->GetMaterialSystem()->GetMaterial(renderer->DefaultMaterial3D_Name)->CreateInstance();
		Terreno->TerrainMaterial->SetBuffer("Camera", renderer->GetUniformBuffers());
		Terreno->TerrainMaterial->SetDynamicBuffer("Animation", BonesUBOs);
		Terreno->TerrainMaterial->SetBuffer("Model", shadowMap->DirShadowsUniformBuffers);
		Terreno->TerrainMaterial->SetTexture("Albedo", Content->DefaultTexture);
		Terreno->TerrainMaterial->SetBuffer("Lights", LightsUniformBuffers);
		Terreno->TerrainMaterial->SetTexture("Specular", Content->DefaultTexture);
		Terreno->TerrainMaterial->SetTexture("Shadows", shadowMap->DirShadows->RenderedSprite.Texture2D);
		Terreno->TerrainMaterial->FlushUpdate();
	}

	void RenderizableScene::DrawShadows(VkCommandBuffer cmdBuffer, uint32_t iteration) {
		PrepareDrawShadows(cmdBuffer, iteration);

		for (const auto& i : Models) {
			DrawShadows(i, cmdBuffer, iteration);
		}

		EndDrawShadows(cmdBuffer, iteration);
	}

	void RenderizableScene::DrawPointShadows(VkCommandBuffer cmdBuffer, uint32_t iteration, CubeShadowMap* map) {
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
		}*/
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
		VULKAN::VulkanImageGen::TransitionImageLayout(&shadowMap->DirShadows->RenderedSprite.Texture2D->Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1, &cmdBuffer);

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

		renderer->SetViewport(cmdBuffer, 0, 0, shadowMap->Size.X, shadowMap->Size.Y);

		renderer->GetMaterialSystem()->GetMaterial(renderer->DefaultShadowsMaterial_Name)->GetGraphicsPipeline(shadowMap->DirShadows->VRenderpass)->Bind(cmdBuffer);
	}
	void RenderizableScene::DrawShadows(Model* model, VkCommandBuffer cmdBuffer, uint32_t i) {
		if (!model->ShadowMaterial.HasValue() || !model->ShadowMaterial->HasBeenSet())
			return;

		model->Bind(cmdBuffer);

		model->ShadowMaterial->GetDescriptorSet()->Bind(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(renderer->DefaultShadowsMaterial_Name)->GetGraphicsPipeline(shadowMap->DirShadows->VRenderpass), i, model->AnimationBufferOffset, BonesUBOs[0].Alignment);

		PushConst3D pushConst = model->GetPushConst();
		vkCmdPushConstants(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(renderer->DefaultShadowsMaterial_Name)->GetGraphicsPipeline(shadowMap->DirShadows->VRenderpass)->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
		model->Draw(cmdBuffer);
	}
	void RenderizableScene::EndDrawShadows(VkCommandBuffer cmdBuffer, uint32_t i) {
		if (Terreno.HasValue() && Terreno->terrainModel != nullptr && Terreno->ShadowsMaterial.HasValue() && Terreno->ShadowsMaterial->HasBeenSet()) {
			Terreno->TerrainMaterial->GetDescriptorSet()->Bind(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(renderer->DefaultShadowsMaterial_Name)->GetGraphicsPipeline(shadowMap->DirShadows->VRenderpass), i, 0, BonesUBOs[0].Alignment);
			Terreno->terrainModel->Bind(cmdBuffer);
			PushConst3D pushConst{ glm::mat4(1.0f) };
			vkCmdPushConstants(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(renderer->DefaultShadowsMaterial_Name)->GetGraphicsPipeline(shadowMap->DirShadows->VRenderpass)->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			Terreno->terrainModel->Draw(cmdBuffer);
		}

		vkCmdEndRenderPass(cmdBuffer);
		VULKAN::VulkanImageGen::TransitionImageLayout(&shadowMap->DirShadows->RenderedSprite.Texture2D->Image, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1, &cmdBuffer);
	}

	void RenderizableScene::PrepareDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
		GraphicsPipeline* pipeline = renderer->GetMaterialSystem()->GetMaterial(renderer->DefaultSkyboxMaterial_Name)->GetGraphicsPipeline(TargetRenderpass);
		pipeline->Bind(cmdBuffer);

		skybox.Bind(cmdBuffer, pipeline, i);
		skybox.Draw(cmdBuffer);
		
		CurrentGraphicsPipeline = renderer->GetMaterialSystem()->GetMaterial(renderer->DefaultMaterial3D_Name)->GetGraphicsPipeline(TargetRenderpass);
		CurrentGraphicsPipeline->Bind(cmdBuffer);

		UpdateLightsBuffers();
	}
	void RenderizableScene::Draw(Model* model, VkCommandBuffer cmdBuffer, uint32_t i) {
		if (!model->ModelMaterial->HasBeenSet())
			return;

		model->Bind(cmdBuffer);

		model->ModelMaterial->GetDescriptorSet()->Bind(cmdBuffer, CurrentGraphicsPipeline, i, model->AnimationBufferOffset, BonesUBOs[0].Alignment);
		
		PushConst3D pushConst = model->GetPushConst();
		vkCmdPushConstants(cmdBuffer, CurrentGraphicsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
		model->Draw(cmdBuffer);
	}
	void RenderizableScene::EndDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
		if (Terreno.HasValue() && Terreno->terrainModel != nullptr) {
			Terreno->TerrainMaterial->GetDescriptorSet()->Bind(cmdBuffer, CurrentGraphicsPipeline, i, 0, BonesUBOs[0].Alignment);
			Terreno->terrainModel->Bind(cmdBuffer);
			PushConst3D pushConst{ glm::mat4(1.0f) };
			vkCmdPushConstants(cmdBuffer, CurrentGraphicsPipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			Terreno->terrainModel->Draw(cmdBuffer);
		}
	}

}
