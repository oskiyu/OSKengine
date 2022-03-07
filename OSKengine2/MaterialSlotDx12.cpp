#include "MaterialSlotDx12.h"

#include "MaterialLayout.h"
#include "GpuUniformBufferDx12.h"
#include "GpuImageDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

MaterialSlotDx12::MaterialSlotDx12(const std::string& name, const MaterialLayout* layout)
	: IMaterialSlot(layout, name) {

}

void MaterialSlotDx12::SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;
	buffers.Insert(index, buffer->As<GpuUniformBufferDx12>());
}

void MaterialSlotDx12::SetGpuImage(const std::string& binding, const GpuImage* image) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;
	images.Insert(index, image->As<GpuImageDx12>());
}

void MaterialSlotDx12::FlushUpdate() {

}

const HashMap<TSize, GpuUniformBufferDx12*>& MaterialSlotDx12::GetUniformBuffers() const {
	return buffers;
}

const HashMap<TSize, GpuImageDx12*>& MaterialSlotDx12::GetGpuImages() const {
	return images;
}
