#include "RenderSystem3D.h"

#include "VulkanRenderer.h"
#include "ModelComponent.h"

using namespace OSK;

Signature RenderSystem3D::GetSystemSignature() {
	Signature signature;

	signature.set(ECSsystem->GetComponentType<OSK::ModelComponent>());

	return signature;
}

void RenderSystem3D::Init() {
	RScene = new RenderizableScene(Renderer);
	Renderer->RSystem = this;
	Stage.Scene = RScene;
	RScene->TargetRenderpass = Renderer->Stage.RTarget->VRenderpass;
}

void RenderSystem3D::OnTick(deltaTime_t deltaTime) {
	for (auto object : Objects) {
		ModelComponent& comp = ECSsystem->GetComponent<ModelComponent>(object);

		for (auto& model : comp.AnimatedModels) {
			model.Update(deltaTime);

			model.UpdateAnimUBO(RScene->BonesUBOs);
		}
	}
}

void RenderSystem3D::OnDraw(VkCommandBuffer cmdBuffer, uint32_t i) {
	if (RScene != nullptr) {
		RScene->UpdateLightsBuffers();

		RScene->PrepareDrawShadows(cmdBuffer, i);

		for (auto object : Objects) {
			ModelComponent& comp = ECSsystem->GetComponent<ModelComponent>(object);

			for (auto& model : comp.StaticMeshes)
				RScene->DrawShadows(&model, cmdBuffer, i);

			for (auto& model : comp.AnimatedModels)
				RScene->DrawShadows(&model, cmdBuffer, i);
		}

		RScene->EndDrawShadows(cmdBuffer, i);
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = Renderer->Stage.RTarget->VRenderpass->VulkanRenderpass;
	renderPassInfo.framebuffer = Renderer->Stage.RTarget->TargetFramebuffers[i]->framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };

	renderPassInfo.renderArea.extent = { Renderer->Stage.RTarget->Size.X, Renderer->Stage.RTarget->Size.Y };
	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0] = { 0.8f, 0.8f, 0.8f, 1.0f }; //Color.
	clearValues[1] = { 1.0f, 0.0f }; //Depth.
	renderPassInfo.clearValueCount = clearValues.size();
	renderPassInfo.pClearValues = clearValues.data();

	Renderer->Stage.RTarget->TransitionToRenderTarget(&cmdBuffer);
	vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	Renderer->SetViewport(cmdBuffer);

	if (RScene != nullptr) {
		RScene->PrepareDraw(cmdBuffer, i);

		for (auto object : Objects) {
			ModelComponent& comp = ECSsystem->GetComponent<ModelComponent>(object);

			for (auto& model : comp.StaticMeshes)
				RScene->Draw(&model, cmdBuffer, i);

			for (auto& model : comp.AnimatedModels)
				RScene->Draw(&model, cmdBuffer, i);
			
		}

		RScene->EndDraw(cmdBuffer, i);
	}

	for (auto& spriteBatch : Stage.SpriteBatches) {

		if (!spriteBatch->spritesToDraw.IsEmpty()) {
			GraphicsPipeline* pipeline = Renderer->GetMaterialSystem()->GetMaterial(Renderer->DefaultMaterial2D_Name)->GetGraphicsPipeline(Renderer->Stage.RTarget->VRenderpass);
			pipeline->Bind(cmdBuffer);

			vkCmdBindIndexBuffer(cmdBuffer, Sprite::IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
			const uint32_t indicesSize = Sprite::Indices.size();

			DescriptorSet* previousDescSet = nullptr;

			for (auto& sprite : spriteBatch->spritesToDraw) {
				if (!sprite.SpriteMaterial->HasBeenSet())
					continue;

				VkBuffer vertexBuffers[] = { sprite.VertexBuffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

				DescriptorSet* newDescSet = sprite.SpriteMaterial->GetDescriptorSet();
				if (previousDescSet != newDescSet) {
					previousDescSet = newDescSet;
					newDescSet->Bind(cmdBuffer, pipeline, i);
				}

				PushConst2D pConst = sprite.PConst;
				vkCmdPushConstants(cmdBuffer, pipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst2D), &pConst);
				vkCmdDrawIndexed(cmdBuffer, indicesSize, 1, 0, 0, 0);
			}
		}
	}

	vkCmdEndRenderPass(cmdBuffer);
	Renderer->Stage.RTarget->TransitionToTexture(&cmdBuffer);
}

void RenderSystem3D::OnRemove() {
	delete RScene;
}
