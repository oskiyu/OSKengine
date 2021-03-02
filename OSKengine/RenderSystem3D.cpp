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
	RScene = new RenderizableScene(Renderer, 1024);
	Renderer->SetRenderizableScene(RScene);
	Renderer->RSystem = this;
	Stage.Scene = RScene;
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

			for (auto& model : comp.StaticMeshes) {
				if (!model.texture->DirShadowsDescriptorSet) {
					RScene->CreateDescriptorSet(&model);
				}

				RScene->DrawShadows(&model, cmdBuffer, i);
			}

			for (auto& model : comp.AnimatedModels) {
				if (!model.texture->DirShadowsDescriptorSet) {
					RScene->CreateDescriptorSet(&model);
				}

				RScene->DrawShadows(&model, cmdBuffer, i);
			}
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

	if (RScene != nullptr) {
		RScene->PrepareDraw(cmdBuffer, i);

		for (auto object : Objects) {
			ModelComponent& comp = ECSsystem->GetComponent<ModelComponent>(object);

			for (auto& model : comp.StaticMeshes) {
				if (!model.texture->PhongDescriptorSet) {
					RScene->CreateDescriptorSet(&model);
				}

				RScene->Draw(&model, cmdBuffer, i);
			}

			for (auto& model : comp.AnimatedModels) {
				if (!model.texture->PhongDescriptorSet) {
					RScene->CreateDescriptorSet(&model);
				}

				RScene->Draw(&model, cmdBuffer, i);
			}
		}

		RScene->EndDraw(cmdBuffer, i);
	}

	for (auto& spriteBatch : Stage.SpriteBatches) {

		if (!spriteBatch->spritesToDraw.empty()) {

			Renderer->Stage.RTarget->Pipelines[0]->Bind(cmdBuffer);
			vkCmdBindIndexBuffer(cmdBuffer, Sprite::IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
			const uint32_t indicesSize = Sprite::Indices.size();

			for (auto& sprite : spriteBatch->spritesToDraw) {
				if (sprite.number == 1) {
					if (sprite.sprites->isOutOfScreen)
						continue;

					VkBuffer vertexBuffers[] = { sprite.sprites->VertexBuffer.Buffer };
					VkDeviceSize offsets[] = { 0 };
					vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

					sprite.sprites->texture->Descriptor->Bind(cmdBuffer, Renderer->Stage.RTarget->Pipelines[0], i);

					PushConst2D pConst = sprite.sprites->getPushConst(spriteBatch->cameraMat);
					vkCmdPushConstants(cmdBuffer, Renderer->Stage.RTarget->Pipelines[0]->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst2D), &pConst);
					vkCmdDrawIndexed(cmdBuffer, indicesSize, 1, 0, 0, 0);
				}
				else {
					for (uint32_t spriteIt = 0; spriteIt < sprite.number; spriteIt++) {
						if (sprite.sprites[spriteIt].texture == nullptr)
							continue;

						if (sprite.sprites[spriteIt].isOutOfScreen)
							continue;

						VkBuffer vertexBuffers[] = { sprite.sprites[spriteIt].VertexBuffer.Buffer };
						VkDeviceSize offsets[] = { 0 };
						vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

						sprite.sprites[spriteIt].texture->Descriptor->Bind(cmdBuffer, Renderer->Stage.RTarget->Pipelines[0], i);

						PushConst2D pConst = sprite.sprites[spriteIt].getPushConst(spriteBatch->cameraMat);
						vkCmdPushConstants(cmdBuffer, Renderer->Stage.RTarget->Pipelines[0]->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst2D), &pConst);
						vkCmdDrawIndexed(cmdBuffer, indicesSize, 1, 0, 0, 0);
					}
				}
			}
		}
	}


	vkCmdEndRenderPass(cmdBuffer);
	Renderer->Stage.RTarget->TransitionToTexture(&cmdBuffer);
}

void RenderSystem3D::OnRemove() {
	delete RScene;
}
