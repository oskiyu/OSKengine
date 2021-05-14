#include "ModelData.h"

using namespace OSK;

void ModelData::Bind(VkCommandBuffer commandBuffer) const {
	const VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.buffer, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
}

void ModelData::Draw(VkCommandBuffer commandBuffer) const {
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesCount), 1, 0, 0, 0);
}