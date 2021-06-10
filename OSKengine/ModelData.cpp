#include "ModelData.h"

using namespace OSK;

void ModelData::Bind(VkCommandBuffer commandBuffer) const {
	const VkDeviceSize offsets[] = { vertexBuffer->memorySubblock->GetOffset() };
	const VkBuffer buffers[] = { vertexBuffer->memorySubblock->GetNativeGpuBuffer() };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer->memorySubblock->GetNativeGpuBuffer(), indexBuffer->memorySubblock->GetOffset(), VK_INDEX_TYPE_UINT32);
}

void ModelData::Draw(VkCommandBuffer commandBuffer) const {
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesCount), 1, 0, 0, 0);
}
