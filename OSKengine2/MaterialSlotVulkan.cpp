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
#include "TopLevelAccelerationStructureVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

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

MaterialSlotVulkan::~MaterialSlotVulkan() {
	pool.Delete();
	descLayout.Delete();
}

void MaterialSlotVulkan::SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) {
	const GpuMemorySubblockVulkan* vulkanBuffer = buffer->GetMemorySubblock()->As<GpuMemorySubblockVulkan>();
	const GpuMemoryBlockVulkan* vulkanBlock = vulkanBuffer->GetOwnerBlock()->As<GpuMemoryBlockVulkan>();

	const bool containsBinding = bindingsLocations.ContainsKey(binding);

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

		if (containsBinding)
			bindings.At(i)[bindingsLocations.Get(binding)] = descriptorWrite;
		else
			bindings.At(i).Insert(descriptorWrite);

		bufferInfos.Insert(bufferInfo);
	}

	if (!containsBinding)
		bindingsLocations.Insert(binding, bindings.At(0).GetSize() - 1);
}

void MaterialSlotVulkan::SetStorageBuffer(const std::string& binding, const GpuDataBuffer* buffer) {
	const GpuMemorySubblockVulkan* vulkanBuffer = buffer->GetMemorySubblock()->As<GpuMemorySubblockVulkan>();
	const GpuMemoryBlockVulkan* vulkanBlock = vulkanBuffer->GetOwnerBlock()->As<GpuMemoryBlockVulkan>();

	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		OwnedPtr<VkDescriptorBufferInfo> bufferInfo = new VkDescriptorBufferInfo();
		bufferInfo->buffer = vulkanBlock->GetVulkanBuffer();
		bufferInfo->offset = vulkanBuffer->GetOffsetFromBlock();
		bufferInfo->range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = layout->GetSlot(name).bindings.Get(binding).glslIndex;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = bufferInfo.GetPointer();
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		if (containsBinding)
			bindings.At(i)[bindingsLocations.Get(binding)] = descriptorWrite;
		else
			bindings.At(i).Insert(descriptorWrite);

		bufferInfos.Insert(bufferInfo);
	}

	if (!containsBinding)
		bindingsLocations.Insert(binding, bindings.At(0).GetSize() - 1);
}

void MaterialSlotVulkan::SetGpuImage(const std::string& binding, const GpuImage* image) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

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

		if (containsBinding)
			bindings.At(i)[bindingsLocations.Get(binding)] = descriptorWrite;
		else
			bindings.At(i).Insert(descriptorWrite);

		imageInfos.Insert(imageInfo);
	}

	if (!containsBinding)
		bindingsLocations.Insert(binding, bindings.At(0).GetSize() - 1);
}

void MaterialSlotVulkan::SetStorageImage(const std::string& binding, const GpuImage* image) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		OwnedPtr<VkDescriptorImageInfo> imageInfo = new VkDescriptorImageInfo();
		imageInfo->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo->imageView = image->As<GpuImageVulkan>()->GetView();
		imageInfo->sampler = image->As<GpuImageVulkan>()->GetSampler();

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = layout->GetSlot(name).bindings.Get(binding).glslIndex;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = imageInfo.GetPointer();
		descriptorWrite.pTexelBufferView = nullptr;
		
		if (containsBinding)
			bindings.At(i)[bindingsLocations.Get(binding)] = descriptorWrite;
		else
			bindings.At(i).Insert(descriptorWrite);

		imageInfos.Insert(imageInfo);
	}

	if (!containsBinding)
		bindingsLocations.Insert(binding, bindings.At(0).GetSize() - 1);
}

void MaterialSlotVulkan::SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	accelerationStructures.Insert(accelerationStructure->As<TopLevelAccelerationStructureVulkan>()->GetAccelerationStructure());

	OwnedPtr<VkWriteDescriptorSetAccelerationStructureKHR> descriptorAccelerationStructureInfo = new VkWriteDescriptorSetAccelerationStructureKHR{};
	descriptorAccelerationStructureInfo->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
	descriptorAccelerationStructureInfo->accelerationStructureCount = 1;
	descriptorAccelerationStructureInfo->pAccelerationStructures = &accelerationStructures.Peek();

	accelerationStructureInfos.Insert(descriptorAccelerationStructureInfo);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.pNext = descriptorAccelerationStructureInfo.GetPointer();
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = layout->GetSlot(name).bindings.Get(binding).glslIndex;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		
		if (containsBinding)
			bindings.At(i)[bindingsLocations.Get(binding)] = descriptorWrite;
		else
			bindings.At(i).Insert(descriptorWrite);
	}

	if (!containsBinding)
		bindingsLocations.Insert(binding, bindings.At(0).GetSize() - 1);
}

void MaterialSlotVulkan::FlushUpdate() {
	for (TSize i = 0; i < bindings.GetSize(); i++)
		vkUpdateDescriptorSets(GetDevice(), (uint32_t)bindings[i].GetSize(), bindings[i].GetData(), 0, nullptr);
}

VkDescriptorSet MaterialSlotVulkan::GetDescriptorSet(TSize index) const {
	return descriptorSets[index];
}
