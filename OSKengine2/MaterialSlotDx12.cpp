#include "MaterialSlotDx12.h"

#include "MaterialLayout.h"
#include "GpuImageDx12.h"
#include "MaterialLayoutSlot.h"

#include "NotImplementedException.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

MaterialSlotDx12::MaterialSlotDx12(const std::string& name, const MaterialLayout* layout)
	: IMaterialSlot(layout, name) {

	buffers.Resize(layout->GetSlot(name).bindings.GetSize(), Pair<USize32, const GpuBuffer*>{ UINT32_MAX, nullptr });
	images.Resize(layout->GetSlot(name).bindings.GetSize(), Pair<USize32, const GpuImageDx12*>{ UINT32_MAX, nullptr });
	storageBuffers.Resize(layout->GetSlot(name).bindings.GetSize(), Pair<USize32, const GpuBuffer*>{ UINT32_MAX, nullptr });
	storageImages.Resize(layout->GetSlot(name).bindings.GetSize(), Pair<USize32, const GpuImageDx12*>{ UINT32_MAX, nullptr });
}
/*
void MaterialSlotDx12::SetUniformBuffers(const std::string& binding, const GpuBuffer* buffer) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;

	if (index >= buffers.GetSize())
		buffers.Resize(index + 1, Pair<TSize, const GpuBuffer*>{ UINT32_MAX, nullptr });

	buffers.At(index) = { index, buffer };
}*/

void MaterialSlotDx12::SetUniformBuffers(const std::string& binding, std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffers) {
	SetUniformBuffer(binding, *buffers[0]);
}
/*
void MaterialSlotDx12::SetGpuImage(const std::string& binding, const IGpuImageView* image) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;

	if (index >= images.GetSize())
		images.Resize(index + 1, Pair<TSize, const GpuImageDx12*>{ UINT32_MAX, nullptr });

	images.At(index) = { index, image->GetImage().As<GpuImageDx12>()};
}
*/
void MaterialSlotDx12::SetGpuImages(const std::string& binding, std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images) {
	SetGpuImage(binding, images[0]);
}
/*
void MaterialSlotDx12::SetStorageImage(const std::string& binding, const IGpuImageView* image) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;

	if (index >= images.GetSize())
		storageImages.Resize(index + 1, Pair<TSize, const GpuImageDx12*>{ UINT32_MAX, nullptr });

	// storageImages.At(index) = { index, image->As<GpuImageDx12>() };
}
*/
void MaterialSlotDx12::SetStorageBuffers(const std::string& binding, std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffer) {
	OSK_ASSERT(false, NotImplementedException());
}

void MaterialSlotDx12::SetStorageImages(const std::string& binding, std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images) {
	SetStorageImage(binding, images[0]);
}

void MaterialSlotDx12::SetAccelerationStructures(const std::string& binding, std::span<const ITopLevelAccelerationStructure*, NUM_RESOURCES_IN_FLIGHT> images) {
	OSK_ASSERT(false, NotImplementedException());
}

void MaterialSlotDx12::FlushUpdate() {

}

const DynamicArray<Pair<UIndex32, const GpuBuffer*>>& MaterialSlotDx12::GetUniformBuffers() const {
	return buffers;
}

const DynamicArray<Pair<UIndex32, const GpuImageDx12*>>& MaterialSlotDx12::GetGpuImages() const {
	return images;
}

const DynamicArray<Pair<UIndex32, const GpuBuffer*>>& MaterialSlotDx12::GetStorageBuffers() const {
	return storageBuffers;
}

const DynamicArray<Pair<UIndex32, const GpuImageDx12*>>& MaterialSlotDx12::GetStorageImages() const {
	return storageImages;
}

void MaterialSlotDx12::SetDebugName(const std::string& name) {
	OSK_ASSERT(false, NotImplementedException());
}
