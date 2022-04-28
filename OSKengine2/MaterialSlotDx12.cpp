#include "MaterialSlotDx12.h"

#include "MaterialLayout.h"
#include "GpuUniformBufferDx12.h"
#include "GpuImageDx12.h"
#include "MaterialLayoutSlot.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

MaterialSlotDx12::MaterialSlotDx12(const std::string& name, const MaterialLayout* layout)
	: IMaterialSlot(layout, name) {

	buffers.Resize(layout->GetSlot(name).bindings.GetSize(), Pair<TSize, const GpuUniformBufferDx12*>{ UINT32_MAX, nullptr });
	images.Resize(layout->GetSlot(name).bindings.GetSize(), Pair<TSize, const GpuImageDx12*>{ UINT32_MAX, nullptr });
}

void MaterialSlotDx12::SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;

	if (index >= buffers.GetSize())
		buffers.Resize(index + 1, Pair<TSize, const GpuUniformBufferDx12*>{ UINT32_MAX, nullptr });

	buffers.At(index) = { index, buffer->As<GpuUniformBufferDx12>() };
}

void MaterialSlotDx12::SetGpuImage(const std::string& binding, const GpuImage* image) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;

	if (index >= images.GetSize())
		images.Resize(index + 1, Pair<TSize, const GpuImageDx12*>{ UINT32_MAX, nullptr });

	images.At(index) = { index, image->As<GpuImageDx12>() };
}

void MaterialSlotDx12::FlushUpdate() {

}

const DynamicArray<Pair<TSize, const GpuUniformBufferDx12*>>& MaterialSlotDx12::GetUniformBuffers() const {
	return buffers;
}

const DynamicArray<Pair<TSize, const GpuImageDx12*>>& MaterialSlotDx12::GetGpuImages() const {
	return images;
}
