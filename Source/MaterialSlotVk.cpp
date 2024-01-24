#include "MaterialSlotVk.h"

#include <vulkan/vulkan.h>
#include "OSKengine.h"
#include "RendererVk.h"
#include "GpuVk.h"
#include "OwnedPtr.h"
#include "GpuBuffer.h"
#include "GpuMemoryBlockVk.h"
#include "GpuMemorySubblockVk.h"
#include "MaterialLayout.h"
#include "DescriptorLayoutVk.h"
#include "DescriptorPoolVk.h"
#include "GpuImageVk.h"
#include "TopLevelAccelerationStructureVk.h"
#include "GpuImageViewVk.h"
#include "MaterialExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

inline VkDevice GetDevice() {
	return Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
}

MaterialSlotVk::MaterialSlotVk(const std::string& name, const MaterialLayout* layout)
	: IMaterialSlot(layout, name),
	m_descLayout(new DescriptorLayoutVk(&layout->GetSlot(name)))
{
	m_pool = new DescriptorPoolVk(m_descLayout.GetValue(), Engine::GetRenderer()->GetSwapchainImagesCount());

	const auto swapchainCount = Engine::GetRenderer()->GetSwapchainImagesCount();

	m_descriptorSets.Resize(swapchainCount);
	m_bindings.Resize(swapchainCount);

	const USize32 maxCount = 1;
	const auto descriptorLayout = m_descLayout->GetLayout();

	VkDescriptorSetVariableDescriptorCountAllocateInfo variableSizeDescriptorInfo{};
	variableSizeDescriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
	variableSizeDescriptorInfo.descriptorSetCount = swapchainCount;
	variableSizeDescriptorInfo.pDescriptorCounts = &maxCount;
	variableSizeDescriptorInfo.pNext = VK_NULL_HANDLE;

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_pool->GetPool();
	allocInfo.descriptorSetCount = swapchainCount;
	allocInfo.pSetLayouts = &descriptorLayout;
	// allocInfo.pNext = &variableSizeDescriptorInfo;

	std::array<const VkDescriptorSetLayout, NUM_RESOURCES_IN_FLIGHT> layouts = { m_descLayout->GetLayout(), m_descLayout->GetLayout(), m_descLayout->GetLayout() };
	allocInfo.pSetLayouts = layouts.data();

	VkResult result = vkAllocateDescriptorSets(GetDevice(), &allocInfo, m_descriptorSets.GetData());
	OSK_ASSERT(result == VK_SUCCESS, MaterialSlotCreationException(result));

	SetDebugName(name);
}

MaterialSlotVk::~MaterialSlotVk() {
	m_pool.Delete();
	m_descLayout.Delete();
}

void MaterialSlotVk::SetUniformBuffers(
	const std::string& binding, 
	std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffer,
	UIndex32 arrayIndex)
{
	const bool containsBinding = m_bindingsLocations.contains(binding);

	for (UIndex32 i = 0; i < m_descriptorSets.GetSize(); i++) {
		const GpuMemorySubblockVk* vulkanBuffer = buffer[i]->GetMemorySubblock()->As<GpuMemorySubblockVk>();
		const GpuMemoryBlockVk* vulkanBlock = vulkanBuffer->GetOwnerBlock()->As<GpuMemoryBlockVk>();

		OwnedPtr<VkDescriptorBufferInfo> bufferInfo = new VkDescriptorBufferInfo();
		bufferInfo->buffer = vulkanBlock->GetVulkanBuffer();
		bufferInfo->offset = vulkanBuffer->GetOffsetFromBlock();
		bufferInfo->range = vulkanBuffer->GetAllocatedSize();

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[i];
		descriptorWrite.dstBinding = m_layout->GetSlot(m_name).bindings.at(binding).glslIndex;
		descriptorWrite.dstArrayElement = arrayIndex;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = bufferInfo.GetPointer();
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		if (containsBinding) {
			m_bindings.At(i)[m_bindingsLocations.at(binding)] = descriptorWrite;
		}
		else {
			m_bindings.At(i).Insert(descriptorWrite);
		}

		m_bufferInfos.Insert(bufferInfo.GetPointer());
	}

	if (!containsBinding) {
		m_bindingsLocations[binding] = static_cast<UIndex32>(m_bindings.At(0).GetSize()) - 1u;
	}
}

void MaterialSlotVk::SetStorageBuffers(
	const std::string& binding, 
	std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffer,
	UIndex32 arrayIndex) 
{
	const bool containsBinding = m_bindingsLocations.contains(binding);

	for (UIndex32 i = 0; i < m_descriptorSets.GetSize(); i++) {
		const GpuMemorySubblockVk* vulkanBuffer = buffer[i]->GetMemorySubblock()->As<GpuMemorySubblockVk>();
		const GpuMemoryBlockVk* vulkanBlock = vulkanBuffer->GetOwnerBlock()->As<GpuMemoryBlockVk>();

		OwnedPtr<VkDescriptorBufferInfo> bufferInfo = new VkDescriptorBufferInfo();
		bufferInfo->buffer = vulkanBlock->GetVulkanBuffer();
		bufferInfo->offset = vulkanBuffer->GetOffsetFromBlock();
		bufferInfo->range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[i];
		descriptorWrite.dstBinding = m_layout->GetSlot(m_name).bindings.at(binding).glslIndex;
		descriptorWrite.dstArrayElement = arrayIndex;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = bufferInfo.GetPointer();
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		if (containsBinding) {
			m_bindings.At(i)[m_bindingsLocations.at(binding)] = descriptorWrite;
		}
		else {
			m_bindings.At(i).Insert(descriptorWrite);
		}

		m_bufferInfos.Insert(bufferInfo.GetPointer());
	}

	if (!containsBinding) {
		m_bindingsLocations[binding] = static_cast<UIndex32>(m_bindings.At(0).GetSize()) - 1;
	}
}

void MaterialSlotVk::SetGpuImages(
	const std::string& binding, 
	std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images,
	UIndex32 arrayIndex) 
{
	const bool containsBinding = m_bindingsLocations.contains(binding);

	for (UIndex32 i = 0; i < m_descriptorSets.GetSize(); i++) {
		OwnedPtr<VkDescriptorImageInfo> imageInfo = new VkDescriptorImageInfo();
		imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo->sampler = images[i]->GetImage().As<GpuImageVk>()->GetVkSampler();
		imageInfo->imageView = images[i]->As<GpuImageViewVk>()->GetVkView();

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[i];
		descriptorWrite.dstBinding = m_layout->GetSlot(m_name).bindings.at(binding).glslIndex;
		descriptorWrite.dstArrayElement = arrayIndex;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = imageInfo.GetPointer();
		descriptorWrite.pTexelBufferView = nullptr;

		if (containsBinding) {
			m_bindings.At(i)[m_bindingsLocations.at(binding)] = descriptorWrite;
		}
		else {
			m_bindings.At(i).Insert(descriptorWrite);
		}

		m_imageInfos.Insert(imageInfo.GetPointer());
	}

	if (!containsBinding) {
		m_bindingsLocations[binding] = static_cast<UIndex32>(m_bindings.At(0).GetSize()) - 1u;
	}
}

void MaterialSlotVk::SetStorageImages(
	const std::string& binding, 
	std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images,
	UIndex32 arrayIndex) 
{
	const bool containsBinding = m_bindingsLocations.contains(binding);

	for (UIndex32 i = 0; i < m_descriptorSets.GetSize(); i++) {
		OwnedPtr<VkDescriptorImageInfo> imageInfo = new VkDescriptorImageInfo();
		imageInfo->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo->sampler = images[i]->GetImage().As<GpuImageVk>()->GetVkSampler();
		imageInfo->imageView = images[i]->As<GpuImageViewVk>()->GetVkView();
		
		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[i];
		descriptorWrite.dstBinding = m_layout->GetSlot(m_name).bindings.at(binding).glslIndex;
		descriptorWrite.dstArrayElement = arrayIndex;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = imageInfo.GetPointer();
		descriptorWrite.pTexelBufferView = nullptr;

		if (containsBinding) {
			m_bindings.At(i)[m_bindingsLocations.at(binding)] = descriptorWrite;
		}
		else {
			m_bindings.At(i).Insert(descriptorWrite);
		}

		m_imageInfos.Insert(imageInfo.GetPointer());
	}

	if (!containsBinding) {
		m_bindingsLocations[binding] = static_cast<USize32>(m_bindings.At(0).GetSize()) - 1u;
	}
}

void MaterialSlotVk::SetAccelerationStructures(
	const std::string& binding, 
	std::span<const ITopLevelAccelerationStructure*, NUM_RESOURCES_IN_FLIGHT> accelerationStructure,
	UIndex32 arrayIndex)
{
	const bool containsBinding = m_bindingsLocations.contains(binding);

	for (UIndex32 i = 0; i < m_descriptorSets.GetSize(); i++) {
		m_accelerationStructures.Insert(accelerationStructure[i]->As<TopLevelAccelerationStructureVk>()->GetAccelerationStructure());

		OwnedPtr<VkWriteDescriptorSetAccelerationStructureKHR> descriptorAccelerationStructureInfo = new VkWriteDescriptorSetAccelerationStructureKHR{};
		descriptorAccelerationStructureInfo->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
		descriptorAccelerationStructureInfo->accelerationStructureCount = 1;
		descriptorAccelerationStructureInfo->pAccelerationStructures = &m_accelerationStructures.Peek();

		m_accelerationStructureInfos.Insert(descriptorAccelerationStructureInfo.GetPointer());

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.pNext = descriptorAccelerationStructureInfo.GetPointer();
		descriptorWrite.dstSet = m_descriptorSets[i];
		descriptorWrite.dstBinding = m_layout->GetSlot(m_name).bindings.at(binding).glslIndex;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;

		if (containsBinding) {
			m_bindings.At(i)[m_bindingsLocations.at(binding)] = descriptorWrite;
		}
		else {
			m_bindings.At(i).Insert(descriptorWrite);
		}
	}

	if (!containsBinding) {
		m_bindingsLocations[binding] = static_cast<UIndex32>(m_bindings.At(0).GetSize()) - 1u;
	}
}

void MaterialSlotVk::FlushUpdate() {
	for (const auto& binding : m_bindings) {
		vkUpdateDescriptorSets(GetDevice(), (uint32_t)binding.GetSize(), binding.GetData(), 0, nullptr);
	}
}

VkDescriptorSet MaterialSlotVk::GetDescriptorSet(UIndex32 index) const {
	return m_descriptorSets[index];
}

void MaterialSlotVk::SetDebugName(const std::string& name) {
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
	nameInfo.pNext = nullptr;

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	nameInfo.pObjectName = name.c_str();

	for (const auto& descSet : m_descriptorSets) {
		nameInfo.objectHandle = (uint64_t)descSet;

		if (RendererVk::pvkSetDebugUtilsObjectNameEXT != nullptr)
			RendererVk::pvkSetDebugUtilsObjectNameEXT(logicalDevice, &nameInfo);
	}
}
