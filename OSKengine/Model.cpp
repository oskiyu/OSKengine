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
		Data->Draw(commandBuffer);
	}

	PushConst3D Model::GetPushConst() {
		ModelTransform.UpdateModel();

		PushConst3D pushConst{};
		pushConst.model = ModelTransform.ModelMatrix;

		return pushConst;
	}

	void Model::UpdateAnimUBO(std::vector<GPUDataBuffer>& buffers) {
		for (auto& i : buffers) {
			i.Write(BonesUBOdata.Bones.data(), sizeof(glm::mat4) * BonesUBOdata.Bones.size(), i.Alignment * AnimationBufferOffset);
		}
	}

}
