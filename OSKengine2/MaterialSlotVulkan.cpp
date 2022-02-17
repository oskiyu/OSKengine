#include "MaterialSlotVulkan.h"

#include <vulkan/vulkan.h>
#include "OSKengine.h"
#include "RendererVulkan.h"
#include "GpuVulkan.h"
#include "OwnedPtr.h"
#include "IGpuUniformBuffer.h"
#include "GpuMemoryBlockVulkan.h"
#include "GpuMemorySubblockVulkan.h"
#include "MaterialLayout.h"

using namespace OSK;

inline VkDevice GetDevice() {
	return Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
}

MaterialSlotVulkan::MaterialSlotVulkan(const std::string& name, const MaterialLayout* layout)
	: IMaterialSlot(layout, name) {

	TSize swapchainCount = 3;// Engine::GetRenderer()->As

	descriptorSets.Resize(swapchainCount);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool->vulkanDescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchainCount);
	allocInfo.pSetLayouts = layouts.data();

	VkResult result = vkAllocateDescriptorSets(GetDevice(), &allocInfo, descriptorSets.GetData());
	OSK_ASSERT(result == VK_SUCCESS, "Error al crear descriptor sets.");
}

void MaterialSlotVulkan::SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) {
	GpuMemorySubblockVulkan* vulkanBuffer = buffer->GetMemorySubblock()->As<GpuMemorySubblockVulkan>();
	GpuMemoryBlockVulkan* vulkanBlock = vulkanBuffer->GetOwnerBlock()->As<GpuMemoryBlockVulkan>();

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		OwnedPtr<VkDescriptorBufferInfo> bufferInfo = new VkDescriptorBufferInfo();
		bufferInfo->buffer = vulkanBlock->GetVulkanBuffer();
		bufferInfo->offset = vulkanBuffer->GetOffsetFromBlock();
		bufferInfo->range = vulkanBuffer->GetAllocatedSize();

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = layout->GetSlot(name).bindings.Get(binding).glslIndex;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = bufferInfo.GetPointer();
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		bindings.At(i).Insert(descriptorWrite);
		bufferInfos.Insert(bufferInfo);
	}
}

void MaterialSlotVulkan::FlushUpdate() {
	for (TSize i = 0; i < bindings.GetSize(); i++)
		vkUpdateDescriptorSets(GetDevice(), bindings.At(i).GetSize(), bindings.At(i).GetData(), 0, nullptr);
}
