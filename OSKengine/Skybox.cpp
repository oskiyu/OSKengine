#include "Skybox.h"

#include "MaterialSlot.h"

namespace OSK {

	void Skybox::Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t i) const {
		Skybox::Model->Bind(commandBuffer);
		instance->GetMaterialSlot(MSLOT_SKYBOX_CAMERA)->GetDescriptorSet()->Bind(commandBuffer, pipeline, i);
		instance->GetMaterialSlot(MSLOT_SKYBOX_TEXTURE)->GetDescriptorSet()->Bind(commandBuffer, pipeline, i);
	}

	void Skybox::Draw(VkCommandBuffer commandBuffer) const {
		Skybox::Model->Draw(commandBuffer);
	}

	ModelData* Skybox::Model = nullptr;

}