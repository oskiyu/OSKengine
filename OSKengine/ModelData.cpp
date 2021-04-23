#include "ModelData.h"

using namespace OSK;

void ModelData::Bind(VkCommandBuffer commandBuffer) const {
	const VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &VertexBuffer.Buffer, offsets);
	vkCmdBindIndexBuffer(commandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
}

void ModelData::Draw(VkCommandBuffer commandBuffer) const {
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(IndicesCount), 1, 0, 0, 0);
}