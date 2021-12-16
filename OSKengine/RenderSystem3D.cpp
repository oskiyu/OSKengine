#include "RenderSystem3D.h"

#include "VulkanRenderer.h"
#include "ModelComponent.h"
#include "MaterialSlot.h"

using namespace OSK;

Signature RenderSystem3D::GetSystemSignature() {
	Signature signature;

	signature.set(entityComponentSystem->GetComponentType<OSK::ModelComponent>());

	return signature;
}

void RenderSystem3D::Init() {
	renderScene = new RenderizableScene(renderer);
	renderer->renderSystem = this;
	renderStage.scene = renderScene.GetPointer();
}

void RenderSystem3D::OnTick(deltaTime_t deltaTime) {
	for (auto object : objects) {
		ModelComponent& comp = entityComponentSystem->GetComponent<ModelComponent>(object);

		for (auto& model : comp.GetAnimatedModels()) {
			model.Update(deltaTime);

			model.UpdateAnimUBO(renderScene->uboBones->GetBuffersRef());
		}
	}
}

void RenderSystem3D::OnDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
	if (renderScene.HasValue()) {
		renderScene->UpdateLightsBuffers();

		renderScene->PrepareDrawShadows(cmdBuffer, i);

		for (auto object : objects) {
			ModelComponent& comp = entityComponentSystem->GetComponent<ModelComponent>(object);

			for (auto& model : comp.GetStaticMeshes())
				renderScene->DrawShadows(&model, cmdBuffer, i);

			for (auto& model : comp.GetAnimatedModels())
				renderScene->DrawShadows(&model, cmdBuffer, i);
		}

		renderScene->EndDrawShadows(cmdBuffer, i);
	}

	renderer->GetMainRenderTarget()->BeginRenderpass(cmdBuffer, i);
	renderer->SetViewport(cmdBuffer);

	if (renderScene.HasValue()) {
		renderScene->PrepareDraw(cmdBuffer, i, renderer->GetMainRenderTarget());

		for (auto object : objects) {
			ModelComponent& comp = entityComponentSystem->GetComponent<ModelComponent>(object);

			for (auto& model : comp.GetStaticMeshes())
				renderScene->Draw(&model, cmdBuffer, i);

			for (auto& model : comp.GetAnimatedModels())
				renderScene->Draw(&model, cmdBuffer, i);
			
		}

		renderScene->EndDraw(cmdBuffer, i);
	}

	vkCmdBindIndexBuffer(cmdBuffer, Sprite::indexBuffer->memorySubblock->GetNativeGpuBuffer(), Sprite::indexBuffer->memorySubblock->GetOffset(), VK_INDEX_TYPE_UINT16);
	const uint32_t indicesSize = (uint32_t)Sprite::indices.size();
	VkBuffer vertexBuffers[] = { Sprite::vertexBuffer->memorySubblock->GetNativeGpuBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

	for (auto& spriteBatch : renderStage.spriteBatches) {

		if (!spriteBatch->spritesToDraw.empty()) {
			GraphicsPipeline* pipeline = renderer->GetMaterialSystem()->GetMaterial(MPIPE_2D)->GetGraphicsPipeline(renderer->GetMainRenderTarget()->renderpass.GetPointer());
			pipeline->Bind(cmdBuffer);

			DescriptorSet* previousDescSet = nullptr;

			for (auto& spritePass : spriteBatch->spritesToDraw) {
				spritePass.camera->GetCameraMaterial()->GetMaterialSlotData(MSLOT_CAMERA_2D)->Bind(cmdBuffer, pipeline, i);

				for (auto& sprite : spritePass.spritesToDraw) {
					if (!sprite.material->HasBeenSet())
						continue;


					DescriptorSet* newDescSet = sprite.material->GetMaterialSlotData(MSLOT_TEXTURE_2D)->GetDescriptorSet();
					if (previousDescSet != newDescSet) {
						previousDescSet = newDescSet;
						sprite.material->GetMaterialSlotData(MSLOT_TEXTURE_2D)->Bind(cmdBuffer, pipeline, i);
					}

					PushConst2D pConst = sprite.getPushConst();
					vkCmdPushConstants(cmdBuffer, pipeline->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst2D), &pConst);
					vkCmdDrawIndexed(cmdBuffer, indicesSize, 1, 0, 0, 0);
				}
			}
		}
	}

	renderer->GetMainRenderTarget()->EndRenderpass();
}
