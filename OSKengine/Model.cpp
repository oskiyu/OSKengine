#include "Model.h"

namespace OSK {

	void Model::Bind(VkCommandBuffer commandBuffer) const {
		data->Bind(commandBuffer);
	}

	void Model::PushConstants(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline) {
		pushConst = GetPushConst();
		vkCmdPushConstants(commandBuffer, pipeline->vulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &pushConst);
	}

	void Model::Draw(VkCommandBuffer commandBuffer) const {
		data->Draw(commandBuffer);
	}

	PushConst3D Model::GetPushConst() {
		PushConst3D pushConst{};
		pushConst.model = transform.AsMatrix();

		return pushConst;
	}

	void Model::UpdateAnimUBO(std::vector<SharedPtr<GpuDataBuffer>>& buffers) {
		for (auto& i : buffers) {
			i->Write(bonesUBOdata.bones.data(), sizeof(glm::mat4) * bonesUBOdata.bones.size(), i->alignment * animationBufferOffset);
		}
	}

}
