#include "MaterialSlotDx12.h"

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "Assert.h"
#include "MaterialLayout.h"
#include "GpuImageDx12.h"
#include "MaterialLayoutSlot.h"

#include "NotImplementedException.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

MaterialSlotDx12::MaterialSlotDx12(const std::string& name, const MaterialLayout* layout)
	: IMaterialSlot(layout, name) {

	buffers.Resize(layout->GetSlot(name).bindings.size(), Pair<USize32, const GpuBuffer*>{ UINT32_MAX, nullptr });
	images.Resize(layout->GetSlot(name).bindings.size(), Pair<USize32, const GpuImageDx12*>{ UINT32_MAX, nullptr });
	storageBuffers.Resize(layout->GetSlot(name).bindings.size(), Pair<USize32, const GpuBuffer*>{ UINT32_MAX, nullptr });
	storageImages.Resize(layout->GetSlot(name).bindings.size(), Pair<USize32, const GpuImageDx12*>{ UINT32_MAX, nullptr });
}


void MaterialSlotDx12::SetUniformBuffer(std::string_view binding, const GpuBuffer& buffer, const GpuBufferRange& range, UIndex32 arrayIndex) {
	OSK_ASSERT(false, NotImplementedException());
}
/*
void MaterialSlotDx12::SetGpuImage(const std::string& binding, const IGpuImageView* image) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;

	if (index >= images.GetSize())
		images.Resize(index + 1, Pair<TSize, const GpuImageDx12*>{ UINT32_MAX, nullptr });

	images.At(index) = { index, image->GetImage().As<GpuImageDx12>()};
}
*/
void MaterialSlotDx12::SetGpuImage(std::string_view binding, const IGpuImageView& view, const IGpuImageSampler& sampler, UIndex32 arrayIndex) {
	OSK_ASSERT(false, NotImplementedException());
}
/*
void MaterialSlotDx12::SetStorageImage(const std::string& binding, const IGpuImageView* image) {
	TSize index = layout->GetSlot(name).bindings.Get(binding).hlslDescriptorIndex;

	if (index >= images.GetSize())
		storageImages.Resize(index + 1, Pair<TSize, const GpuImageDx12*>{ UINT32_MAX, nullptr });

	// storageImages.At(index) = { index, image->As<GpuImageDx12>() };
}
*/
void MaterialSlotDx12::SetStorageBuffer(std::string_view binding, const GpuBuffer& buffer, const GpuBufferRange& range, UIndex32 arrayIndex) {
	OSK_ASSERT(false, NotImplementedException());
}

void MaterialSlotDx12::SetStorageImage(std::string_view binding, const IGpuImageView&, UIndex32 arrayIndex) {
	OSK_ASSERT(false, NotImplementedException());
}

void MaterialSlotDx12::SetAccelerationStructure(std::string_view binding, const ITopLevelAccelerationStructure&, UIndex32 arrayIndex) {
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
	// OSK_ASSERT(false, NotImplementedException()); TODO
}

#endif
