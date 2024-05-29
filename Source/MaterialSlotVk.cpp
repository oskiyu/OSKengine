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

inline static VkDevice GetDevice() {
	return Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
}

MaterialSlotVk::MaterialSlotVk(const std::string& name, const MaterialLayout* layout)
	: IMaterialSlot(layout, name),
	m_descLayout(new DescriptorLayoutVk(&layout->GetSlot(name), 0))
{
	m_pool = new DescriptorPoolVk(m_descLayout.GetValue(), 1);

	std::array<USize32, MAX_RESOURCES_IN_FLIGHT> maxCount{};
	maxCount.fill(1);

	std::array<VkDescriptorSetLayout, MAX_RESOURCES_IN_FLIGHT> descriptorLayout{};
	descriptorLayout.fill(m_descLayout->GetLayout());

	VkDescriptorSetVariableDescriptorCountAllocateInfo variableSizeDescriptorInfo{};
	variableSizeDescriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
	variableSizeDescriptorInfo.descriptorSetCount = static_cast<uint32_t>(m_descriptorSets.size());
	variableSizeDescriptorInfo.pDescriptorCounts = maxCount.data();
	variableSizeDescriptorInfo.pNext = VK_NULL_HANDLE;

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_pool->GetPool();
	allocInfo.descriptorSetCount = m_descriptorSets.size();
	allocInfo.pSetLayouts = descriptorLayout.data();
	allocInfo.pNext = &variableSizeDescriptorInfo;

	std::array<VkDescriptorSetLayout, MAX_RESOURCES_IN_FLIGHT> layouts{};
	layouts.fill(m_descLayout->GetLayout());

	allocInfo.pSetLayouts = layouts.data();

	VkResult result = vkAllocateDescriptorSets(GetDevice(), &allocInfo, m_descriptorSets.data());
	OSK_ASSERT(result == VK_SUCCESS, MaterialSlotCreationException(result));

	SetDebugName(name);
}

MaterialSlotVk::~MaterialSlotVk() {
	m_pool.Delete();
	m_descLayout.Delete();
}

void MaterialSlotVk::SetUniformBuffers(
	const std::string& binding, 
	std::span<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> buffer,
	UIndex32 arrayIndex)
{
	const auto bindingIndexInShader = m_layout->GetSlot(m_name).bindings.at(binding).glslIndex;

	for (UIndex32 frameIdx = 0; frameIdx < m_descriptorSets.size(); frameIdx++) {
		OwnedPtr<VkDescriptorBufferInfo> bufferInfo = GetDescriptorBufferInfo(*buffer[frameIdx]->GetMemorySubblock()->As<GpuMemorySubblockVk>());

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[frameIdx];
		descriptorWrite.dstBinding = bindingIndexInShader;
		descriptorWrite.dstArrayElement = arrayIndex;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = bufferInfo.GetPointer();
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		m_bindings[frameIdx][bindingIndexInShader].descriptorWrites[arrayIndex] = descriptorWrite;

		m_bufferInfos.Insert(bufferInfo.GetPointer());
	}
}

void MaterialSlotVk::SetStorageBuffers(
	const std::string& binding, 
	std::span<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> buffer,
	UIndex32 arrayIndex) 
{
	const auto bindingIndexInShader = m_layout->GetSlot(m_name).bindings.at(binding).glslIndex;

	for (UIndex32 frameIdx = 0; frameIdx < m_descriptorSets.size(); frameIdx++) {
		OwnedPtr<VkDescriptorBufferInfo> bufferInfo = GetDescriptorBufferInfo(*buffer[frameIdx]->GetMemorySubblock()->As<GpuMemorySubblockVk>());

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[frameIdx];
		descriptorWrite.dstBinding = bindingIndexInShader;
		descriptorWrite.dstArrayElement = arrayIndex;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = bufferInfo.GetPointer();
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		m_bindings[frameIdx][bindingIndexInShader].descriptorWrites[arrayIndex] = descriptorWrite;

		m_bufferInfos.Insert(bufferInfo.GetPointer());
	}
}

void MaterialSlotVk::SetGpuImages(
	const std::string& binding, 
	std::span<const IGpuImageView*, MAX_RESOURCES_IN_FLIGHT> images,
	UIndex32 arrayIndex) 
{
	const auto bindingIndexInShader = m_layout->GetSlot(m_name).bindings.at(binding).glslIndex;

	for (UIndex32 frameIndex = 0; frameIndex < m_descriptorSets.size(); frameIndex++) {
		OwnedPtr<VkDescriptorImageInfo> imageInfo = GetDescriptorImageInfo(*images[frameIndex], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = bindingIndexInShader;
		descriptorWrite.dstArrayElement = arrayIndex;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = imageInfo.GetPointer();
		descriptorWrite.pTexelBufferView = nullptr;

		m_bindings[frameIndex][bindingIndexInShader].descriptorWrites[arrayIndex] = descriptorWrite;

		m_imageInfos.Insert(imageInfo.GetPointer());
	}
}

void MaterialSlotVk::SetStorageImages(
	const std::string& binding, 
	std::span<const IGpuImageView*, MAX_RESOURCES_IN_FLIGHT> images,
	UIndex32 arrayIndex) 
{
	const auto bindingIndexInShader = m_layout->GetSlot(m_name).bindings.at(binding).glslIndex;

	for (UIndex32 frameIndex = 0; frameIndex < m_descriptorSets.size(); frameIndex++) {
		OwnedPtr<VkDescriptorImageInfo> imageInfo = GetDescriptorImageInfo(*images[frameIndex], VK_IMAGE_LAYOUT_GENERAL);
		
		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = bindingIndexInShader;
		descriptorWrite.dstArrayElement = arrayIndex;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = imageInfo.GetPointer();
		descriptorWrite.pTexelBufferView = nullptr;

		m_bindings[frameIndex][bindingIndexInShader].descriptorWrites[arrayIndex] = descriptorWrite;

		m_imageInfos.Insert(imageInfo.GetPointer());
	}
}

void MaterialSlotVk::SetAccelerationStructures(
	const std::string& binding, 
	std::span<const ITopLevelAccelerationStructure*, MAX_RESOURCES_IN_FLIGHT> accelerationStructure,
	UIndex32 arrayIndex)
{
	const auto bindingIndexInShader = m_layout->GetSlot(m_name).bindings.at(binding).glslIndex;

	for (UIndex32 frameIndex = 0; frameIndex < m_descriptorSets.size(); frameIndex++) {
		m_accelerationStructures.Insert(accelerationStructure[frameIndex]->As<TopLevelAccelerationStructureVk>()->GetAccelerationStructure());

		OwnedPtr<VkWriteDescriptorSetAccelerationStructureKHR> descriptorAccelerationStructureInfo = new VkWriteDescriptorSetAccelerationStructureKHR{};
		descriptorAccelerationStructureInfo->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
		descriptorAccelerationStructureInfo->accelerationStructureCount = 1;
		descriptorAccelerationStructureInfo->pAccelerationStructures = &m_accelerationStructures.Peek();

		m_accelerationStructureInfos.Insert(descriptorAccelerationStructureInfo.GetPointer());

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.pNext = descriptorAccelerationStructureInfo.GetPointer();
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = m_layout->GetSlot(m_name).bindings.at(binding).glslIndex;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;

		if (!m_bindings[frameIndex].contains(bindingIndexInShader)) {
			m_bindings[frameIndex][bindingIndexInShader] = {};
		}

		auto& bindingWrites = m_bindings[frameIndex][bindingIndexInShader].descriptorWrites;
		bindingWrites[arrayIndex] = descriptorWrite;
	}
}

void MaterialSlotVk::FlushUpdate() {
	VkDevice device = GetDevice();

	m_descriptorWrites.Empty();

	for (const auto& bindingsPerFrame : m_bindings) {
		for (const auto& [bindingIndex, bindingData] : bindingsPerFrame) {
			for (const auto& [arrayIndex, descriptorWrite] : bindingData.descriptorWrites) {
				m_descriptorWrites.Insert(descriptorWrite);
			}
		}
	}

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(m_descriptorWrites.GetSize()), m_descriptorWrites.GetData(), 0, nullptr);
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

	std::string finalName = std::format("{} {}", name, m_name);

	nameInfo.pObjectName = finalName.c_str();

	for (const auto& descSet : m_descriptorSets) {
		nameInfo.objectHandle = (uint64_t)descSet;

		if (RendererVk::pvkSetDebugUtilsObjectNameEXT != nullptr)
			RendererVk::pvkSetDebugUtilsObjectNameEXT(logicalDevice, &nameInfo);
	}
}


OwnedPtr<VkDescriptorBufferInfo> MaterialSlotVk::GetDescriptorBufferInfo(const GpuMemorySubblockVk& subblock) {
	OwnedPtr<VkDescriptorBufferInfo> bufferInfo = new VkDescriptorBufferInfo();

	bufferInfo->buffer = subblock.GetOwnerBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer();
	bufferInfo->offset = subblock.GetOffsetFromBlock();
	bufferInfo->range = subblock.GetAllocatedSize();

	return bufferInfo;
}

OwnedPtr<VkDescriptorImageInfo> MaterialSlotVk::GetDescriptorImageInfo(const IGpuImageView& view, VkImageLayout layout) {
	OwnedPtr<VkDescriptorImageInfo> imageInfo = new VkDescriptorImageInfo();

	imageInfo->imageLayout = layout;
	imageInfo->sampler = view.GetImage().As<GpuImageVk>()->GetVkSampler();
	imageInfo->imageView = view.As<GpuImageViewVk>()->GetVkView();

	return imageInfo;
}
