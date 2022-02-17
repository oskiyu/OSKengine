#include "MaterialSlotDx12.h"

#include "MaterialLayout.h"
#include "GpuUniformBufferDx12.h"

using namespace OSK;

MaterialSlotDx12::MaterialSlotDx12(const std::string& name, const MaterialLayout* layout)
	: IMaterialSlot(layout, name) {

}

void MaterialSlotDx12::SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslIndex;
	buffers.Insert(index, buffer->As<GpuUniformBufferDx12>());
}

void MaterialSlotDx12::FlushUpdate() {

}

const HashMap<TSize, GpuUniformBufferDx12*>& MaterialSlotDx12::GetUniformBuffers() const {
	return buffers;
}
