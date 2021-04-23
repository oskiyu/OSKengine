#include "Skybox.h"

namespace OSK {

	void Skybox::Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t i) const {
		Skybox::Model->Bind(commandBuffer);
		Instance->GetDescriptorSet()->Bind(commandBuffer, pipeline, i);
	}

	void Skybox::Draw(VkCommandBuffer commandBuffer) const {
		Skybox::Model->Draw(commandBuffer);
	}

	ModelData* Skybox::Model = nullptr;

}