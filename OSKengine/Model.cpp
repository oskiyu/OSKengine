#include "Model.h"

namespace OSK {

	void Model::Bind(VkCommandBuffer commandBuffer) const {
		Data->Bind(commandBuffer);
	}

	void Model::PushConstants(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline) {
		PushConst = GetPushConst();
		vkCmdPushConstants(commandBuffer, pipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &PushConst);
	}	

	void Model::Draw(VkCommandBuffer commandBuffer) const {
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(Data->IndicesCount), 1, 0, 0, 0);
	}

	void ModelData::Bind(VkCommandBuffer commandBuffer) const {
		const VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &VertexBuffer.Buffer, offsets);
		vkCmdBindIndexBuffer(commandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	PushConst3D Model::GetPushConst() const {
		if (ModelTransform == nullptr)
			return{};

		PushConst3D pushConst{};
		pushConst.model = ModelTransform->ModelMatrix;

		return pushConst;
	}

}