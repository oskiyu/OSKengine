#include "MaterialInstance.h"

#include "Material.h"
#include "MaterialPool.h"

using namespace OSK;

MaterialInstance::~MaterialInstance() {
	Free();
}

void MaterialInstance::Free() {
	if (ownerPool) {
		GetDescriptorSet()->Reset();
		ownerPool->FreeSet(descriptorSetIndex);

		ownerPool = nullptr;
	}
}

void MaterialInstance::SetTexture(Texture* texture) {
	SetTexture(texture, GetDescriptorSet()->GetBindingsCount());
}

void MaterialInstance::SetTexture(Texture* texture, uint32_t binding) {
	GetDescriptorSet()->AddImage(texture->image.GetPointer(), texture->image->sampler, binding);
}

void MaterialInstance::SetTexture(const std::string& name, Texture* texture) {
	SetTexture(texture, ownerMaterial->GetBindingIndex(name));
}

void MaterialInstance::SetBuffer(std::vector<SharedPtr<GpuDataBuffer>>& buffers) {
	SetBuffer(buffers, GetDescriptorSet()->GetBindingsCount());
}

void MaterialInstance::SetBuffer(std::vector<SharedPtr<GpuDataBuffer>>& buffers, uint32_t binding) {
	GetDescriptorSet()->AddUniformBuffers(buffers, binding, buffers[0]->GetSize());

	if (binding >= this->buffers.size())
		this->buffers.push_back(buffers);
	else
		this->buffers[binding] = buffers;
}

void MaterialInstance::SetBuffer(const std::string& name, std::vector<SharedPtr<GpuDataBuffer>>& buffers) {
	SetBuffer(buffers, ownerMaterial->GetBindingIndex(name));
}

void MaterialInstance::SetDynamicBuffer(std::vector<SharedPtr<GpuDataBuffer>>& buffers) {
	SetDynamicBuffer(buffers, GetDescriptorSet()->GetBindingsCount());
}

void MaterialInstance::SetDynamicBuffer(std::vector<SharedPtr<GpuDataBuffer>>& buffers, uint32_t binding) {
	GetDescriptorSet()->AddDynamicUniformBuffers(buffers, binding, buffers[0]->GetDynamicUboStructureSize());

	if (binding >= this->buffers.size())
		this->buffers.push_back(buffers);
	else
		this->buffers[binding] = buffers;
}

void MaterialInstance::SetDynamicBuffer(const std::string& name, std::vector<SharedPtr<GpuDataBuffer>>& buffers) {
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