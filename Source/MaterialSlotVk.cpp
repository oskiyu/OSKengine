#include "MaterialSlotVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

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
#include "GpuImageSamplerVk.h"

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
	variableSizeDescriptorInfo.descriptorSetCount = static_cast<uint32_t>(1);
	variableSizeDescriptorInfo.pDescriptorCounts = maxCount.data();
	variableSizeDescriptorInfo.pNext = VK_NULL_HANDLE;

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_pool->GetPool();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = descriptorLayout.data();
	allocInfo.pNext = &variableSizeDescriptorInfo;

	std::array<VkDescriptorSetLayout, MAX_RESOURCES_IN_FLIGHT> layouts{};
	layouts.fill(m_descLayout->GetLayout());

	allocInfo.pSetLayouts = layouts.data();

	VkResult result = vkAllocateDescriptorSets(GetDevice(), &allocInfo, &m_descriptorSet);
	OSK_ASSERT(result == VK_SUCCESS, MaterialSlotCreationException(result));

	SetDebugName(name);
}

MaterialSlotVk::~MaterialSlotVk() {
	m_pool.Delete();
	m_descLayout.Delete();
}

void MaterialSlotVk::SetUniformBuffer(
	std::string_view binding, 
	const GpuBuffer& buffer, 
	const GpuBufferRange& range,
	UIndex32 arrayIndex)
{
	const UIndex32 bindingIndexInShader = GetLayout()->GetSlot(GetName()).bindings.find(binding)->second.glslIndex;

	OwnedPtr<VkDescriptorBufferInfo> bufferInfo = GetDescriptorBufferInfo(*buffer.GetMemorySubblock()->As<GpuMemorySubblockVk>(), range);

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSet;
	descriptorWrite.dstBinding = bindingIndexInShader;
	descriptorWrite.dstArrayElement = arrayIndex;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = bufferInfo.GetPointer();
	descriptorWrite.pImageInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;

	m_bindings[bindingIndexInShader].descriptorWrites[arrayIndex] = descriptorWrite;
	m_bindings[bindingIndexInShader].bufferInfos[arrayIndex] = bufferInfo.GetPointer();
	m_bindings[bindingIndexInShader].dirtyDescriptorWrites.insert(arrayIndex);
}

void MaterialSlotVk::SetStorageBuffer(
	std::string_view binding,
	const GpuBuffer& buffer,
	const GpuBufferRange& range,
	UIndex32 arrayIndex)
{
	const UIndex32 bindingIndexInShader = GetLayout()->GetSlot(GetName()).bindings.find(binding)->second.glslIndex;

	OwnedPtr<VkDescriptorBufferInfo> bufferInfo = GetDescriptorBufferInfo(*buffer.GetMemorySubblock()->As<GpuMemorySubblockVk>(), range);

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSet;
	descriptorWrite.dstBinding = bindingIndexInShader;
	descriptorWrite.dstArrayElement = arrayIndex;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = bufferInfo.GetPointer();
	descriptorWrite.pImageInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;

	m_bindings[bindingIndexInShader].descriptorWrites[arrayIndex] = descriptorWrite;
	m_bindings[bindingIndexInShader].bufferInfos[arrayIndex] = bufferInfo.GetPointer();
	m_bindings[bindingIndexInShader].dirtyDescriptorWrites.insert(arrayIndex);
}

void MaterialSlotVk::SetGpuImage(
	std::string_view binding,
	const IGpuImageView& image,
	const IGpuImageSampler& sampler,
	UIndex32 arrayIndex)
{
	const UIndex32 bindingIndexInShader = GetLayout()->GetSlot(GetName()).bindings.find(binding)->second.glslIndex;

	OwnedPtr<VkDescriptorImageInfo> imageInfo = GetDescriptorImageInfo(image, sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSet;
	descriptorWrite.dstBinding = bindingIndexInShader;
	descriptorWrite.dstArrayElement = arrayIndex;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = nullptr;
	descriptorWrite.pImageInfo = imageInfo.GetPointer();
	descriptorWrite.pTexelBufferView = nullptr;

	m_bindings[bindingIndexInShader].descriptorWrites[arrayIndex] = descriptorWrite;
	m_bindings[bindingIndexInShader].imageInfos[arrayIndex] = imageInfo.GetPointer();
	m_bindings[bindingIndexInShader].dirtyDescriptorWrites.insert(arrayIndex);
}

void MaterialSlotVk::SetStorageImage(
	std::string_view binding,
	const IGpuImageView& image,
	UIndex32 arrayIndex)
{
	const UIndex32 bindingIndexInShader = GetLayout()->GetSlot(GetName()).bindings.find(binding)->second.glslIndex;

	OwnedPtr<VkDescriptorImageInfo> imageInfo = GetDescriptorImageInfo(image, VK_IMAGE_LAYOUT_GENERAL);

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSet;
	descriptorWrite.dstBinding = bindingIndexInShader;
	descriptorWrite.dstArrayElement = arrayIndex;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = nullptr;
	descriptorWrite.pImageInfo = imageInfo.GetPointer();
	descriptorWrite.pTexelBufferView = nullptr;

	m_bindings[bindingIndexInShader].descriptorWrites[arrayIndex] = descriptorWrite;
	m_bindings[bindingIndexInShader].imageInfos[arrayIndex] = imageInfo.GetPointer();
	m_bindings[bindingIndexInShader].dirtyDescriptorWrites.insert(arrayIndex);
}

void MaterialSlotVk::SetAccelerationStructure(
	std::string_view binding,
	const ITopLevelAccelerationStructure& accelerationStructure,
	UIndex32 arrayIndex)
{
	const UIndex32 bindingIndexInShader = GetLayout()->GetSlot(GetName()).bindings.find(binding)->second.glslIndex;

	m_accelerationStructures.Insert(accelerationStructure.As<TopLevelAccelerationStructureVk>()->GetAccelerationStructure());

	OwnedPtr<VkWriteDescriptorSetAccelerationStructureKHR> descriptorAccelerationStructureInfo = new VkWriteDescriptorSetAccelerationStructureKHR{};
	descriptorAccelerationStructureInfo->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
	descriptorAccelerationStructureInfo->accelerationStructureCount = 1;
	descriptorAccelerationStructureInfo->pAccelerationStructures = &m_accelerationStructures.Peek();

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.pNext = descriptorAccelerationStructureInfo.GetPointer();
	descriptorWrite.dstSet = m_descriptorSet;
	descriptorWrite.dstBinding = bindingIndexInShader;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;

	m_bindings[bindingIndexInShader].descriptorWrites[arrayIndex] = descriptorWrite;
	m_bindings[bindingIndexInShader].asInfos[arrayIndex] = descriptorAccelerationStructureInfo.GetPointer();
	m_bindings[bindingIndexInShader].dirtyDescriptorWrites.insert(arrayIndex);
}

void MaterialSlotVk::FlushUpdate() {
	VkDevice device = GetDevice();

	m_descriptorWrites.Empty();

	for (auto& [bindingIndex, bindingData] : m_bindings) {

		for (UIndex32 dirtyIndex : bindingData.dirtyDescriptorWrites) {
			const VkWriteDescriptorSet data = bindingData.descriptorWrites.at(dirtyIndex);
			m_descriptorWrites.Insert(data);
		}
		// for (const auto& [arrayIndex, descriptorWrite] : bindingData.descriptorWrites) {
		//	m_descriptorWrites.Insert(descriptorWrite);
		// }

		bindingData.dirtyDescriptorWrites.clear();
	}

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(m_descriptorWrites.GetSize()), m_descriptorWrites.GetData(), 0, nullptr);
}

void MaterialSlotVk::SetDebugName(const std::string& name) {
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
	nameInfo.pNext = nullptr;

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	std::string finalName = std::format("{} {}", name, GetName());

	nameInfo.pObjectName = finalName.c_str();

	nameInfo.objectHandle = (uint64_t)(m_descriptorSet);

	if (RendererVk::pvkSetDebugUtilsObjectNameEXT != nullptr) {
		RendererVk::pvkSetDebugUtilsObjectNameEXT(logicalDevice, &nameInfo);
	}

}

VkDescriptorSet MaterialSlotVk::GetDescriptorSet() const {
	return m_descriptorSet;
}

OwnedPtr<VkDescriptorBufferInfo> MaterialSlotVk::GetDescriptorBufferInfo(const GpuMemorySubblockVk& subblock, const GpuBufferRange& range) {
	OwnedPtr<VkDescriptorBufferInfo> bufferInfo = new VkDescriptorBufferInfo();

	bufferInfo->buffer = subblock.GetOwnerBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer();

	if (range.full) {
		bufferInfo->offset = subblock.GetOffsetFromBlock();
		bufferInfo->range = subblock.GetAllocatedSize();
	}
	else {
		bufferInfo->offset = subblock.GetOffsetFromBlock() + range.offset;
		bufferInfo->range = range.size;
	}

	return bufferInfo;
}

OwnedPtr<VkDescriptorImageInfo> MaterialSlotVk::GetDescriptorImageInfo(const IGpuImageView& view, const IGpuImageSampler& sampler, VkImageLayout layout) {
	OwnedPtr<VkDescriptorImageInfo> imageInfo = GetDescriptorImageInfo(view, layout);

	imageInfo->sampler = sampler.As<GpuImageSamplerVk>()->GetSamplerVk();

	return imageInfo;
}

OwnedPtr<VkDescriptorImageInfo> MaterialSlotVk::GetDescriptorImageInfo(const IGpuImageView& view, VkImageLayout layout) {
	OwnedPtr<VkDescriptorImageInfo> imageInfo = new VkDescriptorImageInfo();

	imageInfo->imageLayout = layout;
	imageInfo->sampler = VK_NULL_HANDLE;
	imageInfo->imageView = view.As<GpuImageViewVk>()->GetVkView();

	return imageInfo;
}

#endif
