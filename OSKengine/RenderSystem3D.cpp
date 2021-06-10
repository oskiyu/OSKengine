#include "RenderSystem3D.h"

#include "VulkanRenderer.h"
#include "ModelComponent.h"

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
	renderScene->targetRenderpass = renderer->GetMainRenderTarget()->vulkanRenderpass;
}

void RenderSystem3D::OnTick(deltaTime_t deltaTime) {
	for (auto object : objects) {
		ModelComponent& comp = entityComponentSystem->GetComponent<ModelComponent>(object);

		for (auto& model : comp.GetAnimatedModels()) {
			model.Update(deltaTime);

			model.UpdateAnimUBO(renderScene->bonesUbos);
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

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderer->GetMainRenderTarget()->vulkanRenderpass->vulkanRenderpass;
	renderPassInfo.framebuffer = renderer->GetMainRenderTarget()->targetFramebuffers[i]->framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };

	renderPassInfo.renderArea.extent = { renderer->GetMainRenderTarget()->GetSize().X, renderer->GetMainRenderTarget()->GetSize().Y };
	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0] = { 0.8f, 0.8f, 0.8f, 1.0f }; //Color.
	clearValues[1] = { 1.0f, 0.0f }; //Depth.
	renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
	renderPassInfo.pClearValues = clearValues.data();

	renderer->GetMainRenderTarget()->TransitionToRenderTarget(&cmdBuffer);
	vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	renderer->SetViewport(cmdBuffer);

	if (renderScene.HasValue()) {
		renderScene->PrepareDraw(cmdBuffer, i);

		for (auto object : objects) {
			ModelComponent& comp = entityComponentSystem->GetComponent<ModelComponent>(object);

			for (auto& model : comp.GetStaticMeshes())
				renderScene->Draw(&model, cmdBuffer, i);

			for (auto& model : comp.GetAnimatedModels())
				renderScene->Draw(&model, cmdBuffer, i);
			
		}

		renderScene->EndDraw(cmdBuffer, i);
	}

	for (auto& spriteBatch : renderStage.spriteBatches) {

		if (!spriteBatch->spritesToDraw.IsEmpty()) {
			GraphicsPipeline* pipeline = renderer->GetMaterialSystem()->GetMaterial(renderer->defaultMaterial2D_Name)->GetGraphicsPipeline(renderer->GetMainRenderTarget()->vulkanRenderpass);
			pipeline->Bind(cmdBuffer);

			vkCmdBindIndexBuffer(cmdBuffer, Sprite::indexBuffer->memorySubblock->GetNativeGpuBuffer(), Sprite::indexBuffer->memorySubblock->GetOffset(), VK_INDEX_TYPE_UINT16);
			const uint32_t indicesSize = (uint32_t)Sprite::indices.size();

			DescriptorSet* previousDescSet = nullptr;

			for (auto& sprite : spriteBatch->spritesToDraw) {
				if (!sprite.spriteMaterial->HasBeenSet())
					continue;

				VkBuffer vertexBuffers[] = { sprite.vertexBuffer };
				VkDeviceSize offsets[] = { sprite.bufferOffset };
				vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

				DescriptorSet* newDescSet = sprite.spriteMaterial->GetDescriptorSet();
				if (previousDescSet != newDescSet) {
					previousDescSet = newDescSet;
					newDescSet->Bind(cmdBuffer, pipeline, i);
				}

				PushConst2D pConst = sprite.pushConst;
				vkCmdPushConstants(cmdBuffer, pipeline->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst2D), &pConst);
				vkCmdDrawIndexed(cmdBuffer, indicesSize, 1, 0, 0, 0);
			}
		}
	}

	vkCmdEndRenderPass(cmdBuffer);
	renderer->GetMainRenderTarget()->TransitionToTexture(&cmdBuffer);
}
