#include "RenderizableScene.h"

#include "VulkanRenderer.h"
#include "DescriptorSet.h"

#include "MaterialSlot.h"

namespace OSK {

	RenderizableScene::RenderizableScene(RenderAPI* renderer) {
		this->renderer = renderer;
		
		content = new ContentManager(this->renderer);
		shadowMap = new ShadowMap(renderer, content.GetPointer());
		shadowMap->Create({ 1024, 1024 });
		//shadowMap->dirShadows->targetImage = RenderTargetImageTarget::DEPTH;

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
		uboLights->GetBuffersRef().resize(renderer->swapchain->GetImageCount());
		for (uint32_t i = 0; i < uboLights->GetBuffers().size(); i++) {
			uboLights->GetBuffersRef()[i] = new GpuDataBuffer;
			renderer->AllocateBuffer(uboLights->GetBuffers()[i].GetPointer(), size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}

		uboBones->GetBuffersRef().resize(renderer->swapchain->GetImageCount());
		for (uint32_t i = 0; i < uboBones->GetBuffers().size(); i++) {
			uboBones->GetBuffersRef()[i] = renderer->CreateDynamicUBO(sizeof(AnimUBO), 64);

			AnimUBO ubo{};
			uboBones->GetBuffersRef()[i]->Write(&ubo, sizeof(AnimUBO));
		}
	}

	void RenderizableScene::UpdateLightsBuffers() {
		for (auto& i : uboLights->GetBuffers())
			lights.UpdateBuffer(renderer->logicalDevice, i.Get());

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
		content->LoadSkybox(&skybox, path);
	}

	void RenderizableScene::LoadHeightmap(const std::string& path, const Vector2f& quadSize, float maxHeight) {
		terreno = new Terrain(content.GetPointer());

		terreno->CreateMesh(path, quadSize, maxHeight);

		terreno->material = renderer->GetMaterialSystem()->GetMaterial(MPIPE_3D)->CreateInstance().GetPointer();
		terreno->material->GetMaterialSlot(MSLOT_CAMERA_3D)->SetBuffer("Camera", renderer->GetDefaultCamera()->GetUniformBuffer());

		terreno->material->GetMaterialSlot(MSLOT_SCENE_3D)->SetDynamicBuffer("DirLightMat", shadowMap->uboDirLightMat);
		terreno->material->GetMaterialSlot(MSLOT_SCENE_3D)->SetBuffer("Lights", uboLights);

		terreno->material->GetMaterialSlot(MSLOT_PER_MODEL_3D)->SetTexture("Albedo", content->LoadTexture("heightmaps/color.png"));
		terreno->material->GetMaterialSlot(MSLOT_PER_MODEL_3D)->SetTexture("Specular", content->LoadTexture("heightmaps/color.png"));

		terreno->material->GetMaterialSlot(MSLOT_PER_INSTANCE_3D)->SetDynamicBuffer("Bones", uboBones);
		
		terreno->material->FlushUpdate();
	}

	void RenderizableScene::DrawShadows(VkCommandBuffer cmdBuffer, uint32_t iteration) {
		PrepareDrawShadows(cmdBuffer, iteration);

		for (const auto& i : models) {
			DrawShadows(i, cmdBuffer, iteration);
		}

		EndDrawShadows(cmdBuffer, iteration);
	}

	void RenderizableScene::Draw(VkCommandBuffer cmdBuffer, uint32_t iteration, RenderTarget* target) {
		PrepareDraw(cmdBuffer, iteration, target);

		for (const auto& i : models)
			Draw(i, cmdBuffer, iteration);

		EndDraw(cmdBuffer, iteration);
	}

	//SYSTEM

	void RenderizableScene::PrepareDrawShadows(VkCommandBuffer cmdBuffer, uint32_t i) {
		shadowMap->dirShadows->BeginRenderpass(cmdBuffer, i);

		renderer->SetViewport(cmdBuffer, 0, 0, shadowMap->GetImageSize().X, shadowMap->GetImageSize().Y);

		renderer->GetMaterialSystem()->GetMaterial(MPIPE_SHADOWS3D)->GetGraphicsPipeline(shadowMap->dirShadows->renderpass.GetPointer())->Bind(cmdBuffer);
	}
	void RenderizableScene::DrawShadows(Model* model, VkCommandBuffer cmdBuffer, uint32_t i) {
		if (!model->shadowMaterial.HasValue() || !model->shadowMaterial->HasBeenSet())
			return;

		model->Bind(cmdBuffer);

		model->shadowMaterial->GetMaterialSlotData(MSLOT_SHADOWS_3D_CAMERA)->Bind(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(MPIPE_SHADOWS3D)->GetGraphicsPipeline(shadowMap->dirShadows->renderpass.GetPointer()), i);
		model->shadowMaterial->GetMaterialSlotData(MSLOT_SHADOWS_3D_SCENE)->Bind(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(MPIPE_SHADOWS3D)->GetGraphicsPipeline(shadowMap->dirShadows->renderpass.GetPointer()), i);
		model->shadowMaterial->GetMaterialSlotData(MSLOT_SHADOWS_3D_BONES)->Bind(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(MPIPE_SHADOWS3D)->GetGraphicsPipeline(shadowMap->dirShadows->renderpass.GetPointer()), i, model->animationBufferOffset, uboBones->GetBufferAlignment());

		PushConst3D pushConst = model->GetPushConst();
		vkCmdPushConstants(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(MPIPE_SHADOWS3D)->GetGraphicsPipeline(shadowMap->dirShadows->renderpass.GetPointer())->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
		model->Draw(cmdBuffer);
	}
	void RenderizableScene::EndDrawShadows(VkCommandBuffer cmdBuffer, uint32_t i) {
		if (terreno.HasValue() && terreno->terrainModel != nullptr && terreno->shadowMaterial.HasValue() && terreno->shadowMaterial->HasBeenSet()) {
			terreno->shadowMaterial->GetMaterialSlotData(MSLOT_SHADOWS_3D_CAMERA)->Bind(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(MPIPE_SHADOWS3D)->GetGraphicsPipeline(shadowMap->dirShadows->renderpass.GetPointer()), i);
			terreno->shadowMaterial->GetMaterialSlotData(MSLOT_SHADOWS_3D_SCENE)->Bind(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(MPIPE_SHADOWS3D)->GetGraphicsPipeline(shadowMap->dirShadows->renderpass.GetPointer()), i);
			terreno->shadowMaterial->GetMaterialSlotData(MSLOT_SHADOWS_3D_BONES)->Bind(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(MPIPE_SHADOWS3D)->GetGraphicsPipeline(shadowMap->dirShadows->renderpass.GetPointer()), i, 0, uboBones->GetBufferAlignment());

			terreno->terrainModel->Bind(cmdBuffer);
			PushConst3D pushConst{ glm::mat4(1.0f) };
			vkCmdPushConstants(cmdBuffer, renderer->GetMaterialSystem()->GetMaterial(MPIPE_SHADOWS3D)->GetGraphicsPipeline(shadowMap->dirShadows->renderpass.GetPointer())->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			terreno->terrainModel->Draw(cmdBuffer);
		}
		
		shadowMap->dirShadows->EndRenderpass();
	}

	void RenderizableScene::PrepareDraw(VkCommandBuffer cmdBuffer, uint32_t i, RenderTarget* target) {
		GraphicsPipeline* pipeline = renderer->GetMaterialSystem()->GetMaterial(MPIPE_SKYBOX)->GetGraphicsPipeline(target->renderpass.GetPointer());
		pipeline->Bind(cmdBuffer);

		if (skybox.instance.HasValue()) {
			skybox.Bind(cmdBuffer, pipeline, i);
			skybox.Draw(cmdBuffer);
		}
		
		currentGraphicsPipeline = renderer->GetMaterialSystem()->GetMaterial(MPIPE_3D)->GetGraphicsPipeline(target->renderpass.GetPointer());
		currentGraphicsPipeline->Bind(cmdBuffer);

		UpdateLightsBuffers();
	}
	void RenderizableScene::Draw(Model* model, VkCommandBuffer cmdBuffer, uint32_t i) {
		if (!model->material->HasBeenSet())
			return;

		model->Bind(cmdBuffer);

		model->material->GetMaterialSlotData(MSLOT_CAMERA_3D)->Bind(cmdBuffer, currentGraphicsPipeline, i);
		model->material->GetMaterialSlotData(MSLOT_SCENE_3D)->Bind(cmdBuffer, currentGraphicsPipeline, i);
		model->material->GetMaterialSlotData(MSLOT_PER_MODEL_3D)->Bind(cmdBuffer, currentGraphicsPipeline, i);
		model->material->GetMaterialSlotData(MSLOT_PER_INSTANCE_3D)->Bind(cmdBuffer, currentGraphicsPipeline, i, model->animationBufferOffset, uboBones->GetBufferAlignment());
		
		PushConst3D pushConst = model->GetPushConst();
		vkCmdPushConstants(cmdBuffer, currentGraphicsPipeline->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
		model->Draw(cmdBuffer);
	}
	void RenderizableScene::EndDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
		if (terreno.HasValue() && terreno->terrainModel != nullptr) {
			terreno->material->GetMaterialSlotData(MSLOT_CAMERA_3D)->Bind(cmdBuffer, currentGraphicsPipeline, i);
			terreno->material->GetMaterialSlotData(MSLOT_SCENE_3D)->Bind(cmdBuffer, currentGraphicsPipeline, i);
			terreno->material->GetMaterialSlotData(MSLOT_PER_MODEL_3D)->Bind(cmdBuffer, currentGraphicsPipeline, i);
			terreno->material->GetMaterialSlotData(MSLOT_PER_INSTANCE_3D)->Bind(cmdBuffer, currentGraphicsPipeline, i, 0, uboBones->GetBufferAlignment());

			terreno->terrainModel->Bind(cmdBuffer);
			PushConst3D pushConst{ glm::mat4(1.0f) };
			vkCmdPushConstants(cmdBuffer, currentGraphicsPipeline->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
			terreno->terrainModel->Draw(cmdBuffer);
		}
	}

}
