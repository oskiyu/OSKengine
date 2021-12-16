#include "MaterialSlot.h"

#include "MaterialSlotPool.h"
#include "Material.h"

using namespace OSK;


void MaterialSlot::SetTexture(uint32_t binding, Texture* texture) {
	bindings.SetTexture(binding, texture);
}
void MaterialSlot::SetTexture(uint32_t binding, SharedPtr<OSK::VULKAN::GpuImage> image) {
	bindings.SetTexture(binding, image);
}
void MaterialSlot::SetTexture(const std::string& bindingName, Texture* texture) {
	SetTexture(material->GetBindingNumber(slotType, bindingName), texture);
}
void MaterialSlot::SetTexture(const std::string& bindingName, SharedPtr<OSK::VULKAN::GpuImage> image) {
	SetTexture(material->GetBindingNumber(slotType, bindingName), image);
}
void MaterialSlot::SetBuffer(uint32_t binding, SharedPtr<UniformBuffer> ubo) {
	bindings.SetBuffer(binding, ubo);
}
void MaterialSlot::SetBuffer(const std::string& bindingName, SharedPtr<UniformBuffer> ubo) {
	bindings.SetBuffer(material->GetBindingNumber(slotType, bindingName), ubo);
}
void MaterialSlot::SetDynamicBuffer(uint32_t binding, SharedPtr<UniformBuffer> ubo) {
	bindings.SetDynamicBuffer(binding, ubo);
}
void MaterialSlot::SetDynamicBuffer(const std::string& bindingName, SharedPtr<UniformBuffer> ubo) {
	bindings.SetDynamicBuffer(material->GetBindingNumber(slotType, bindingName), ubo);
}

void MaterialSlot::FlushUpdate() {
	handler = material->GetHandler(slotType, bindings);
}
MaterialSlotHandler MaterialSlot::GetHandler() const {
	return handler;
}



void MaterialSlotData::SetTexture(uint32_t binding, Texture* texture) {
	GetDescriptorSet()->AddImage(texture->image.GetPointer(), texture->image->sampler, binding);
}

void MaterialSlotData::SetTexture(uint32_t binding, SharedPtr<OSK::VULKAN::GpuImage> image) {
	GetDescriptorSet()->AddImage(image.GetPointer(), image->sampler, binding);
}

void MaterialSlotData::SetTexture(uint32_t binding, OSK::VULKAN::GpuImage* image) {
	GetDescriptorSet()->AddImage(image, image->sampler, binding);
}

void MaterialSlotData::SetTexture(const std::string& bindingName, Texture* texture) {
	SetTexture(GetDescriptorSet()->GetDescriptorLayout()->GetBindingFromName(bindingName), texture);
}

void MaterialSlotData::SetTexture(const std::string& bindingName, SharedPtr<OSK::VULKAN::GpuImage> image) {
	SetTexture(GetDescriptorSet()->GetDescriptorLayout()->GetBindingFromName(bindingName), image);
}

void MaterialSlotData::SetTexture(const std::string& bindingName, OSK::VULKAN::GpuImage* image) {
	SetTexture(GetDescriptorSet()->GetDescriptorLayout()->GetBindingFromName(bindingName), image);
}

void MaterialSlotData::SetBuffer(uint32_t binding, SharedPtr<UniformBuffer> ubo) {
	GetDescriptorSet()->AddUniformBuffers(ubo->GetBuffers(), binding, ubo->GetBufferSize());

	if (binding >= this->buffers.size())
		this->buffers.push_back(ubo);
	else
		this->buffers[binding] = ubo;
}

void MaterialSlotData::SetBuffer(const std::string& bindingName, SharedPtr<UniformBuffer> ubo) {
	SetBuffer(GetDescriptorSet()->GetDescriptorLayout()->GetBindingFromName(bindingName), ubo);
}

void MaterialSlotData::SetDynamicBuffer(uint32_t binding, SharedPtr<UniformBuffer> ubo) {
	GetDescriptorSet()->AddDynamicUniformBuffers(ubo->GetBuffers(), binding, ubo->GetBufferSize());

	if (binding >= this->buffers.size())
		this->buffers.push_back(ubo);
	else
		this->buffers[binding] = ubo;
}

void MaterialSlotData::SetDynamicBuffer(const std::string& bindingName, SharedPtr<UniformBuffer> ubo) {
	SetDynamicBuffer(GetDescriptorSet()->GetDescriptorLayout()->GetBindingFromName(bindingName), ubo);
}

DescriptorSet* MaterialSlotData::GetDescriptorSet() const {
	return pool->GetDescriptorSet(index);
}

void MaterialSlotData::FlushUpdate() {
	GetDescriptorSet()->Update();

	hasBeenSet = true;
}

bool MaterialSlotData::HasBeenSet() const {
	return hasBeenSet;
}

void MaterialSlotData::Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t iteration) {
	GetDescriptorSet()->Bind(commandBuffer, pipeline, iteration, material->GetDescriptorSetNumber(slotType));
}

void MaterialSlotData::Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t iteration, uint32_t dynamicOffset, uint32_t alignment) {
	GetDescriptorSet()->Bind(commandBuffer, pipeline, iteration, material->GetDescriptorSetNumber(slotType), dynamicOffset, alignment);
}
