#include "IMaterialSlot.h"

#include "Texture.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IMaterialSlot::IMaterialSlot(const MaterialLayout* layout, const std::string& name)
	: layout(layout), name(name) {

}

void IMaterialSlot::SetTexture(const std::string& binding, const ASSETS::Texture* texture, SampledChannel channel) {
	SetGpuImage(binding, texture->GetGpuImage()->GetView(GpuImageViewConfig::CreateSampled_Default()));
}

void IMaterialSlot::SetTextures(const std::string& binding, const ASSETS::Texture* texture[NUM_RESOURCES_IN_FLIGHT], SampledChannel channel) {
	const auto viewConfig = GpuImageViewConfig::CreateSampled_Default();
	const IGpuImageView* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = texture[i]->GetGpuImage()->GetView(viewConfig);

	SetGpuImages(binding, images);
}

void IMaterialSlot::SetUniformBuffer(const std::string& binding, const GpuBuffer& buffer) {
	const GpuBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		buffers[i] = &buffer;

	SetUniformBuffers(binding, buffers);
}

void IMaterialSlot::SetGpuImage(const std::string& binding, const IGpuImageView* image) {
	const IGpuImageView* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = image;

	SetGpuImages(binding, images);
}

void IMaterialSlot::SetStorageBuffer(const std::string& binding, const GpuBuffer* buffer) {
	const GpuBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		buffers[i] = buffer;

	SetStorageBuffers(binding, buffers);
}

void IMaterialSlot::SetStorageImage(const std::string& binding, const IGpuImageView* image) {
	const IGpuImageView* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = image;

	SetStorageImages(binding, images);
}

void IMaterialSlot::SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure) {
	const ITopLevelAccelerationStructure* accelerationStructures[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		accelerationStructures[i] = accelerationStructure;

	SetAccelerationStructures(binding, accelerationStructures);
}

std::string_view IMaterialSlot::GetName() const {
	return name;
}
