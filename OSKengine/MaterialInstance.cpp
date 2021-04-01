#include "MaterialInstance.h"

#include "Material.h"
#include "MaterialPool.h"

using namespace OSK;

MaterialInstance::~MaterialInstance() {
	Free();
}

void MaterialInstance::Free() {
	if (OwnerPool) {
		GetDescriptorSet()->Reset();
		OwnerPool->FreeSet(DSet);
	}
}

void MaterialInstance::SetTexture(Texture* texture) {
	SetTexture(texture, GetDescriptorSet()->GetBindingsCount());
}

void MaterialInstance::SetTexture(Texture* texture, uint32_t binding) {
	GetDescriptorSet()->AddImage(&texture->Image, texture->Image.Sampler, binding);
}

void MaterialInstance::SetTexture(const std::string& name, Texture* texture) {
	SetTexture(texture, OwnerMaterial->GetBindingIndex(name));
}

void MaterialInstance::SetBuffer(std::vector<VulkanBuffer>& buffers) {
	SetBuffer(buffers, GetDescriptorSet()->GetBindingsCount());
}

void MaterialInstance::SetBuffer(std::vector<VulkanBuffer>& buffers, uint32_t binding) {
	GetDescriptorSet()->AddUniformBuffers(buffers, binding, buffers[0].Size);
}

void MaterialInstance::SetBuffer(const std::string& name, std::vector<VulkanBuffer>& buffers) {
	SetBuffer(buffers, OwnerMaterial->GetBindingIndex(name));
}

void MaterialInstance::SetDynamicBuffer(std::vector<VulkanBuffer>& buffers) {
	SetDynamicBuffer(buffers, GetDescriptorSet()->GetBindingsCount());
}

void MaterialInstance::SetDynamicBuffer(std::vector<VulkanBuffer>& buffers, uint32_t binding) {
	GetDescriptorSet()->AddDynamicUniformBuffers(buffers, binding, buffers[0].DynamicSize);
}

void MaterialInstance::SetDynamicBuffer(const std::string& name, std::vector<VulkanBuffer>& buffers) {
	SetDynamicBuffer(buffers, OwnerMaterial->GetBindingIndex(name));
}

void MaterialInstance::FlushUpdate() {
	GetDescriptorSet()->Update();
	hasBeenSet = true;
}

bool MaterialInstance::HasBeenSet() const {
	return hasBeenSet;
}

DescriptorSet* MaterialInstance::GetDescriptorSet() const {
	return OwnerPool->GetDSet(DSet);
}