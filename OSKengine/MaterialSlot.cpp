#include "MaterialSlot.h"

#include "MaterialSlotPool.h"
#include "Material.h"

using namespace OSK;

void MaterialSlot::SetTexture(uint32_t binding, Texture* texture) {
	GetDescriptorSet()->AddImage(texture->image.GetPointer(), texture->image->sampler, binding);
}

void MaterialSlot::SetTexture(uint32_t binding, SharedPtr<OSK::VULKAN::GpuImage> image) {
	GetDescriptorSet()->AddImage(image.GetPointer(), image->sampler, binding);
}

void MaterialSlot::SetTexture(uint32_t binding, OSK::VULKAN::GpuImage* image) {
	GetDescriptorSet()->AddImage(image, image->sampler, binding);
}

void MaterialSlot::SetTexture(const std::string& bindingName, Texture* texture) {	
	SetTexture(GetDescriptorSet()->GetDescriptorLayout()->GetBindingFromName(bindingName), texture);
}

void MaterialSlot::SetTexture(const std::string& bindingName, SharedPtr<OSK::VULKAN::GpuImage> image) {
	SetTexture(GetDescriptorSet()->GetDescriptorLayout()->GetBindingFromName(bindingName), image);
}

void MaterialSlot::SetTexture(const std::string& bindingName, OSK::VULKAN::GpuImage* image) {
	SetTexture(GetDescriptorSet()->GetDescriptorLayout()->GetBindingFromName(bindingName), image);
}

void MaterialSlot::SetBuffer(uint32_t binding, const UniformBuffer& ubo) {
	GetDescriptorSet()->AddUniformBuffers(ubo.GetBuffers(), binding, ubo.GetBufferSize());

	if (binding >= this->buffers.size())
		this->buffers.push_back(ubo);
	else
		this->buffers[binding] = ubo;
}

void MaterialSlot::SetBuffer(const std::string& bindingName, const UniformBuffer& ubo) {
	SetBuffer(GetDescriptorSet()->GetDescriptorLayout()->GetBindingFromName(bindingName), ubo);
}

void MaterialSlot::SetDynamicBuffer(uint32_t binding, const UniformBuffer& ubo) {
	GetDescriptorSet()->AddDynamicUniformBuffers(ubo.GetBuffers(), binding, ubo.GetBufferSize());

	if (binding >= this->buffers.size())
		this->buffers.push_back(ubo);
	else
		this->buffers[binding] = ubo;
}

void MaterialSlot::SetDynamicBuffer(const std::string& bindingName, const UniformBuffer& ubo) {
	SetDynamicBuffer(GetDescriptorSet()->GetDescriptorLayout()->GetBindingFromName(bindingName), ubo);
}

DescriptorSet* MaterialSlot::GetDescriptorSet() const {
	return pool->GetDescriptorSet(index);
}

void MaterialSlot::FlushUpdate() {
	GetDescriptorSet()->Update();

	hasBeenSet = true;
}

bool MaterialSlot::HasBeenSet() const {
	return hasBeenSet;
}
