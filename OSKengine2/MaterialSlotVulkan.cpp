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
#include "DescriptorLayoutVulkan.h"
#include "DescriptorPoolVulkan.h"
#include "GpuImageVulkan.h"

using namespace OSK;

inline VkDevice GetDevice() {
	return Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice();
}

MaterialSlotVulkan::MaterialSlotVulkan(const std::string& name, const MaterialLayout* layout)
	: IMaterialSlot(layout, name) {

	TSize swapchainCount = Engine::GetRenderer()->GetSwapchainImagesCount();

	descriptorSets.Resize(swapchainCount);
	bindings.Resize(3);

	descLayout = new DescriptorLayoutVulkan(&layout->GetSlot(name));
	pool = new DescriptorPoolVulkan(descLayout.GetValue(), swapchainCount);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool->GetPool();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchainCount);

	VkDescriptorSetLayout layouts[] = { descLayout->GetLayout(), descLayout->GetLayout(), descLayout->GetLayout() };
	allocInfo.pSetLayouts = layouts;

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

void MaterialSlotVulkan::SetGpuImage(const std::string& binding, const GpuImage* image) {
	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		OwnedPtr<VkDescriptorImageInfo> imageInfo = new VkDescriptorImageInfo();
		imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo->imageView = image->As<GpuImageVulkan>()->GetView();
		imageInfo->sampler = image->As<GpuImageVulkan>()->GetSampler();

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = layout->GetSlot(name).bindings.Get(binding).glslIndex;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = imageInfo.GetPointer();
		descriptorWrite.pTexelBufferView = nullptr;

		bindings[i].Insert(descriptorWrite);
		imageInfos.Insert(imageInfo);
	}
}

void MaterialSlotVulkan::FlushUpdate() {
	for (TSize i = 0; i < bindings.GetSize(); i++)
		vkUpdateDescriptorSets(GetDevice(), (uint32_t)bindings[i].GetSize(), bindings[i].GetData(), 0, nullptr);
}

VkDescriptorSet MaterialSlotVulkan::GetDescriptorSet(TSize index) const {
	return descriptorSets[index];
}
