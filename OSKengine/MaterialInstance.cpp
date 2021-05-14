#include "MaterialInstance.h"

#include "Material.h"
#include "MaterialPool.h"

using namespace OSK;

MaterialInstance::~MaterialInstance() {
	try {
		Free();
	}
	catch(std::runtime_error _) {

	}
}

void MaterialInstance::Free() {
	if (ownerPool) {
		GetDescriptorSet()->Reset();
		ownerPool->FreeSet(descriptorSetIndex);
	}
}

void MaterialInstance::SetTexture(Texture* texture) {
	SetTexture(texture, GetDescriptorSet()->GetBindingsCount());
}

void MaterialInstance::SetTexture(Texture* texture, uint32_t binding) {
	GetDescriptorSet()->AddImage(&texture->image, texture->image.sampler, binding);
}

void MaterialInstance::SetTexture(const std::string& name, Texture* texture) {
	SetTexture(texture, ownerMaterial->GetBindingIndex(name));
}

void MaterialInstance::SetBuffer(std::vector<GPUDataBuffer>& buffers) {
	SetBuffer(buffers, GetDescriptorSet()->GetBindingsCount());
}

void MaterialInstance::SetBuffer(std::vector<GPUDataBuffer>& buffers, uint32_t binding) {
	GetDescriptorSet()->AddUniformBuffers(buffers, binding, buffers[0].GetSize());
}

void MaterialInstance::SetBuffer(const std::string& name, std::vector<GPUDataBuffer>& buffers) {
	SetBuffer(buffers, ownerMaterial->GetBindingIndex(name));
}

void MaterialInstance::SetDynamicBuffer(std::vector<GPUDataBuffer>& buffers) {
	SetDynamicBuffer(buffers, GetDescriptorSet()->GetBindingsCount());
}

void MaterialInstance::SetDynamicBuffer(std::vector<GPUDataBuffer>& buffers, uint32_t binding) {
	GetDescriptorSet()->AddDynamicUniformBuffers(buffers, binding, buffers[0].GetDynamicUboStructureSize());
}

void MaterialInstance::SetDynamicBuffer(const std::string& name, std::vector<GPUDataBuffer>& buffers) {
	SetDynamicBuffer(buffers, ownerMaterial->GetBindingIndex(name));
}

void MaterialInstance::FlushUpdate() {
	GetDescriptorSet()->Update();
	hasBeenSet = true;
}

bool MaterialInstance::HasBeenSet() const {
	return hasBeenSet;
}

DescriptorSet* MaterialInstance::GetDescriptorSet() const {
	return ownerPool->GetDSet(descriptorSetIndex);
}