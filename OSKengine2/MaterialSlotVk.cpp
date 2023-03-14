#include "MaterialSlotVk.h"

#include <vulkan/vulkan.h>
#include "OSKengine.h"
#include "RendererVk.h"
#include "GpuVk.h"
#include "OwnedPtr.h"
#include "IGpuUniformBuffer.h"
#include "GpuMemoryBlockVk.h"
#include "GpuMemorySubblockVk.h"
#include "MaterialLayout.h"
#include "DescriptorLayoutVk.h"
#include "DescriptorPoolVk.h"
#include "GpuImageVk.h"
#include "TopLevelAccelerationStructureVk.h"
#include "GpuImageViewVk.h"
#include "IGpuStorageBuffer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

inline VkDevice GetDevice() {
	return Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
}

MaterialSlotVk::MaterialSlotVk(const std::string& name, const MaterialLayout* layout)
	: IMaterialSlot(layout, name) {

	const TSize swapchainCount = Engine::GetRenderer()->GetSwapchainImagesCount();

	descriptorSets.Resize(swapchainCount);
	bindings.Resize(3);

	descLayout = new DescriptorLayoutVk(&layout->GetSlot(name));
	pool = new DescriptorPoolVk(descLayout.GetValue(), swapchainCount);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool->GetPool();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchainCount);

	VkDescriptorSetLayout layouts[] = { descLayout->GetLayout(), descLayout->GetLayout(), descLayout->GetLayout() };
	allocInfo.pSetLayouts = layouts;

	VkResult result = vkAllocateDescriptorSets(GetDevice(), &allocInfo, descriptorSets.GetData());
	OSK_ASSERT(result == VK_SUCCESS, "Error al crear descriptor sets.");

	SetDebugName(name);
}

MaterialSlotVk::~MaterialSlotVk() {
	pool.Delete();
	descLayout.Delete();
}

void MaterialSlotVk::SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) {
	const IGpuUniformBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		buffers[i] = buffer;

	SetUniformBuffers(binding, buffers);
}

void MaterialSlotVk::SetUniformBuffers(const std::string& binding, const IGpuUniformBuffer* buffer[NUM_RESOURCES_IN_FLIGHT]) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		const GpuMemorySubblockVk* vulkanBuffer = buffer[i]->GetMemorySubblock()->As<GpuMemorySubblockVk>();
		const GpuMemoryBlockVk* vulkanBlock = vulkanBuffer->GetOwnerBlock()->As<GpuMemoryBlockVk>();

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

		bufferInfos.Insert(bufferInfo.GetPointer());
	}

	if (!containsBinding)
		bindingsLocations.Insert(binding, bindings.At(0).GetSize() - 1);
}

void MaterialSlotVk::SetStorageBuffer(const std::string& binding, const GpuDataBuffer* buffer) {
	const GpuDataBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		buffers[i] = buffer;

	SetStorageBuffers(binding, buffers);
}

void MaterialSlotVk::SetStorageBuffers(const std::string& binding, const GpuDataBuffer* buffer[NUM_RESOURCES_IN_FLIGHT]) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		const GpuMemorySubblockVk* vulkanBuffer = buffer[i]->GetMemorySubblock()->As<GpuMemorySubblockVk>();
		const GpuMemoryBlockVk* vulkanBlock = vulkanBuffer->GetOwnerBlock()->As<GpuMemoryBlockVk>();

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

		bufferInfos.Insert(bufferInfo.GetPointer());
	}

	if (!containsBinding)
		bindingsLocations.Insert(binding, bindings.At(0).GetSize() - 1);
}

void MaterialSlotVk::SetGpuImage(const std::string& binding, const GpuImage* image, SampledChannel channel, SampledArrayType arrayType, TSize arrayLevel) {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = image;

	SetGpuImages(binding, images, channel, arrayType, arrayLevel);
}

void MaterialSlotVk::SetGpuImages(const std::string& binding, const GpuImage* image[NUM_RESOURCES_IN_FLIGHT], SampledChannel channel, SampledArrayType arrayType, TSize arrayLevel) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		OwnedPtr<VkDescriptorImageInfo> imageInfo = new VkDescriptorImageInfo();
		imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo->sampler = image[i]->As<GpuImageVk>()->GetVkSampler();
		imageInfo->imageView = image[i]->GetView(channel, arrayType, arrayLevel, image[i]->GetNumLayers(), ViewUsage::SAMPLED)
			->As<GpuImageViewVk>()->GetVkView();

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

		imageInfos.Insert(imageInfo.GetPointer());
	}

	if (!containsBinding)
		bindingsLocations.Insert(binding, bindings.At(0).GetSize() - 1);
}

void MaterialSlotVk::SetStorageImage(const std::string& binding, const GpuImage* image, SampledArrayType arrayType, TSize arrayLevel) {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = image;

	SetStorageImages(binding, images, arrayType, arrayLevel);
}

void MaterialSlotVk::SetStorageImages(const std::string& binding, const GpuImage* image[NUM_RESOURCES_IN_FLIGHT], SampledArrayType arrayType, TSize arrayLevel) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		OwnedPtr<VkDescriptorImageInfo> imageInfo = new VkDescriptorImageInfo();
		imageInfo->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo->sampler = image[i]->As<GpuImageVk>()->GetVkSampler();
		imageInfo->imageView = image[i]->GetView(SampledChannel::COLOR, arrayType, arrayLevel, image[i]->GetNumLayers(), ViewUsage::STORAGE)
			->As<GpuImageViewVk>()->GetVkView();
		
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

		imageInfos.Insert(imageInfo.GetPointer());
	}

	if (!containsBinding)
		bindingsLocations.Insert(binding, bindings.At(0).GetSize() - 1);
}

void MaterialSlotVk::SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure) {
	const ITopLevelAccelerationStructure* accelerationStructures[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		accelerationStructures[i] = accelerationStructure;

	SetAccelerationStructures(binding, accelerationStructures);
}

void MaterialSlotVk::SetAccelerationStructures(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure[NUM_RESOURCES_IN_FLIGHT]) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		accelerationStructures.Insert(accelerationStructure[i]->As<TopLevelAccelerationStructureVk>()->GetAccelerationStructure());

		OwnedPtr<VkWriteDescriptorSetAccelerationStructureKHR> descriptorAccelerationStructureInfo = new VkWriteDescriptorSetAccelerationStructureKHR{};
		descriptorAccelerationStructureInfo->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
		descriptorAccelerationStructureInfo->accelerationStructureCount = 1;
		descriptorAccelerationStructureInfo->pAccelerationStructures = &accelerationStructures.Peek();

		accelerationStructureInfos.Insert(descriptorAccelerationStructureInfo.GetPointer());

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

void MaterialSlotVk::FlushUpdate() {
	for (TSize i = 0; i < bindings.GetSize(); i++)
		vkUpdateDescriptorSets(GetDevice(), (uint32_t)bindings[i].GetSize(), bindings[i].GetData(), 0, nullptr);
}

VkDescriptorSet MaterialSlotVk::GetDescriptorSet(TSize index) const {
	return descriptorSets[index];
}

void MaterialSlotVk::SetDebugName(const std::string& name) {
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
	nameInfo.pNext = nullptr;

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	nameInfo.pObjectName = name.c_str();

	for (TIndex i = 0; i < descriptorSets.GetSize(); i++) {
		nameInfo.objectHandle = (uint64_t)descriptorSets[i];

		if (RendererVk::pvkSetDebugUtilsObjectNameEXT != nullptr)
			RendererVk::pvkSetDebugUtilsObjectNameEXT(logicalDevice, &nameInfo);
	}
}
