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
}

void RenderSystem3D::OnTick(deltaTime_t deltaTime) {
	for (auto object : Objects) {
		ModelComponent& comp = ECSsystem->GetComponent<ModelComponent>(object);

		for (auto& model : comp.AnimatedModels) {
			model.Update(deltaTime);

			model.UpdateAnimUBO();
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

		//for (auto& cubeMap : stage->Scene->cubeShadowMaps)
			//stage->Scene->DrawPointShadows(cmdBuffer, iteration, cubeMap);
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

			for (auto& model : comp.StaticMeshes)
				RScene->Draw(&model, cmdBuffer, i);

			for (auto& model : comp.AnimatedModels)
				RScene->Draw(&model, cmdBuffer, i);
		}

		RScene->EndDraw(cmdBuffer, i);
	}

	vkCmdEndRenderPass(cmdBuffer);
	Renderer->Stage.RTarget->TransitionToTexture(&cmdBuffer);
}

void RenderSystem3D::OnRemove() {
	delete RScene;
}
