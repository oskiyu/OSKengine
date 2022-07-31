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
	const IGpuUniformBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		buffers[i] = buffer;

	SetUniformBuffers(binding, buffers);
}

void MaterialSlotVulkan::SetUniformBuffers(const std::string& binding, const IGpuUniformBuffer* buffer[NUM_RESOURCES_IN_FLIGHT]) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		const GpuMemorySubblockVulkan* vulkanBuffer = buffer[i]->GetMemorySubblock()->As<GpuMemorySubblockVulkan>();
		const GpuMemoryBlockVulkan* vulkanBlock = vulkanBuffer->GetOwnerBlock()->As<GpuMemoryBlockVulkan>();

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

void MaterialSlotVulkan::SetStorageBuffer(const std::string& binding, const GpuDataBuffer* buffer) {
	const GpuDataBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		buffers[i] = buffer;

	SetStorageBuffers(binding, buffers);
}

void MaterialSlotVulkan::SetStorageBuffers(const std::string& binding, const GpuDataBuffer* buffer[NUM_RESOURCES_IN_FLIGHT]) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		const GpuMemorySubblockVulkan* vulkanBuffer = buffer[i]->GetMemorySubblock()->As<GpuMemorySubblockVulkan>();
		const GpuMemoryBlockVulkan* vulkanBlock = vulkanBuffer->GetOwnerBlock()->As<GpuMemoryBlockVulkan>();

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

void MaterialSlotVulkan::SetGpuImage(const std::string& binding, const GpuImage* image, SampledChannel channel, TSize arrayLevel) {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = image;

	SetGpuImages(binding, images, channel, arrayLevel);
}

void MaterialSlotVulkan::SetGpuImages(const std::string& binding, const GpuImage* image[NUM_RESOURCES_IN_FLIGHT], SampledChannel channel, TSize arrayLevel) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		OwnedPtr<VkDescriptorImageInfo> imageInfo = new VkDescriptorImageInfo();
		imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo->sampler = image[i]->As<GpuImageVulkan>()->GetVkSampler();

		switch (channel) {
		case SampledChannel::COLOR:
			imageInfo->imageView = image[i]->As<GpuImageVulkan>()->GetColorView(arrayLevel);
			break;
		case SampledChannel::DEPTH:
			imageInfo->imageView = image[i]->As<GpuImageVulkan>()->GetDepthOnlyView(arrayLevel);
			OSK_ASSERT(imageInfo->imageView != VK_NULL_HANDLE, "No se puede usar la imagen para leer la parte de profundidad.");
			break;
		case SampledChannel::STENCIL:
			imageInfo->imageView = image[i]->As<GpuImageVulkan>()->GetStencilOnlyView(arrayLevel);
			OSK_ASSERT(imageInfo->imageView != VK_NULL_HANDLE, "No se puede usar la imagen para leer la parte de stencil.");
			break;
		}

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

void MaterialSlotVulkan::SetGpuArrayImage(const std::string& binding, const GpuImage* image, SampledChannel channel) {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = image;

	SetGpuArrayImages(binding, images, channel);
}

void MaterialSlotVulkan::SetGpuArrayImages(const std::string& binding, const GpuImage* images[NUM_RESOURCES_IN_FLIGHT], SampledChannel channel) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		OwnedPtr<VkDescriptorImageInfo> imageInfo = new VkDescriptorImageInfo();
		imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo->sampler = images[i]->As<GpuImageVulkan>()->GetVkSampler();

		switch (channel) {
		case SampledChannel::COLOR:
			imageInfo->imageView = images[i]->As<GpuImageVulkan>()->GetColorArrayView();
			break;
		case SampledChannel::DEPTH:
			imageInfo->imageView = images[i]->As<GpuImageVulkan>()->GetDepthArrayView();
			OSK_ASSERT(imageInfo->imageView != VK_NULL_HANDLE, "No se puede usar la imagen para leer la parte de profundidad.");
			break;
		case SampledChannel::STENCIL:
			imageInfo->imageView = images[i]->As<GpuImageVulkan>()->GetStencilArrayView();
			OSK_ASSERT(imageInfo->imageView != VK_NULL_HANDLE, "No se puede usar la imagen para leer la parte de stencil.");
			break;
		}

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

void MaterialSlotVulkan::SetStorageImage(const std::string& binding, const GpuImage* image) {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = image;

	SetStorageImages(binding, images);
}

void MaterialSlotVulkan::SetStorageImages(const std::string& binding, const GpuImage* image[NUM_RESOURCES_IN_FLIGHT]) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		OwnedPtr<VkDescriptorImageInfo> imageInfo = new VkDescriptorImageInfo();
		imageInfo->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo->imageView = image[i]->As<GpuImageVulkan>()->GetColorView(0);
		imageInfo->sampler = image[i]->As<GpuImageVulkan>()->GetVkSampler();

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

void MaterialSlotVulkan::SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure) {
	const ITopLevelAccelerationStructure* accelerationStructures[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		accelerationStructures[i] = accelerationStructure;

	SetAccelerationStructures(binding, accelerationStructures);
}

void MaterialSlotVulkan::SetAccelerationStructures(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure[NUM_RESOURCES_IN_FLIGHT]) {
	const bool containsBinding = bindingsLocations.ContainsKey(binding);

	for (TSize i = 0; i < descriptorSets.GetSize(); i++) {
		accelerationStructures.Insert(accelerationStructure[i]->As<TopLevelAccelerationStructureVulkan>()->GetAccelerationStructure());

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

void MaterialSlotVulkan::FlushUpdate() {
	for (TSize i = 0; i < bindings.GetSize(); i++)
		vkUpdateDescriptorSets(GetDevice(), (uint32_t)bindings[i].GetSize(), bindings[i].GetData(), 0, nullptr);
}

VkDescriptorSet MaterialSlotVulkan::GetDescriptorSet(TSize index) const {
	return descriptorSets[index];
}
