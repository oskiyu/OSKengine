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
	storageBuffers.Resize(layout->GetSlot(name).bindings.GetSize(), Pair<TSize, const IGpuStorageBuffer*>{ UINT32_MAX, nullptr });
	storageImages.Resize(layout->GetSlot(name).bindings.GetSize(), Pair<TSize, const GpuImageDx12*>{ UINT32_MAX, nullptr });
}

void MaterialSlotDx12::SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;

	if (index >= buffers.GetSize())
		buffers.Resize(index + 1, Pair<TSize, const GpuUniformBufferDx12*>{ UINT32_MAX, nullptr });

	buffers.At(index) = { index, buffer->As<GpuUniformBufferDx12>() };
}

void MaterialSlotDx12::SetUniformBuffers(const std::string& binding, const IGpuUniformBuffer* buffer[NUM_RESOURCES_IN_FLIGHT]) {
	SetUniformBuffer(binding, buffer[0]);
}

void MaterialSlotDx12::SetGpuImage(const std::string& binding, const GpuImage* image, SampledChannel channel, SampledArrayType arrayType, TSize arrayLevel) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;

	if (index >= images.GetSize())
		images.Resize(index + 1, Pair<TSize, const GpuImageDx12*>{ UINT32_MAX, nullptr });

	images.At(index) = { index, image->As<GpuImageDx12>() };
}

void MaterialSlotDx12::SetGpuImages(const std::string& binding, const GpuImage* image[NUM_RESOURCES_IN_FLIGHT], SampledChannel channel, SampledArrayType arrayType, TSize arrayLevel) {
	SetGpuImage(binding, image[0], channel, arrayType, arrayLevel);
}

void MaterialSlotDx12::SetStorageBuffer(const std::string& binding, const GpuDataBuffer* buffer) {
	OSK_ASSERT(false, "No implementado.");
}

void MaterialSlotDx12::SetStorageImage(const std::string& binding, const GpuImage* image, SampledArrayType arrayType, TSize arrayLayer) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;

	if (index >= images.GetSize())
		storageImages.Resize(index + 1, Pair<TSize, const GpuImageDx12*>{ UINT32_MAX, nullptr });

	storageImages.At(index) = { index, image->As<GpuImageDx12>() };
}

void MaterialSlotDx12::SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* image) {
	OSK_ASSERT(false, "No implementado.");
}

void MaterialSlotDx12::SetStorageBuffers(const std::string& binding, const GpuDataBuffer* buffer[NUM_RESOURCES_IN_FLIGHT]) {
	OSK_ASSERT(false, "No implementado.");
}

void MaterialSlotDx12::SetStorageImages(const std::string& binding, const GpuImage* image[NUM_RESOURCES_IN_FLIGHT], SampledArrayType arrayType, TSize arrayLayer) {
	SetStorageImage(binding, image[0], arrayType, arrayLayer);
}

void MaterialSlotDx12::SetAccelerationStructures(const std::string& binding, const ITopLevelAccelerationStructure* image[NUM_RESOURCES_IN_FLIGHT]) {
	OSK_ASSERT(false, "No implementado.");
}

void MaterialSlotDx12::FlushUpdate() {

}

const DynamicArray<Pair<TSize, const GpuUniformBufferDx12*>>& MaterialSlotDx12::GetUniformBuffers() const {
	return buffers;
}

const DynamicArray<Pair<TSize, const GpuImageDx12*>>& MaterialSlotDx12::GetGpuImages() const {
	return images;
}

const DynamicArray<Pair<TSize, const IGpuStorageBuffer*>>& MaterialSlotDx12::GetStorageBuffers() const {
	return storageBuffers;
}

const DynamicArray<Pair<TSize, const GpuImageDx12*>>& MaterialSlotDx12::GetStorageImages() const {
	return storageImages;
}
