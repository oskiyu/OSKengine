#include "IMaterialSlot.h"

#include "Texture.h"
#include <array>

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IMaterialSlot::IMaterialSlot(const MaterialLayout* layout, const std::string& name)
	: layout(layout), name(name) {

}

void IMaterialSlot::SetTexture(const std::string& binding, const Texture* texture, SampledChannel channel) {
	SetGpuImage(binding, texture->GetGpuImage()->GetView(GpuImageViewConfig::CreateSampled_Default()));
}

void IMaterialSlot::SetTextures(const std::string& binding, std::span<const Texture*, NUM_RESOURCES_IN_FLIGHT> textures, SampledChannel channel) {
	auto viewConfig = GpuImageViewConfig::CreateSampled_Default();
	viewConfig.channel = channel;

	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = textures[i]->GetGpuImage()->GetView(viewConfig);

	SetGpuImages(binding, images);
}

void IMaterialSlot::SetUniformBuffer(const std::string& binding, const GpuBuffer& buffer) {
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffers{};

	for (auto& b : buffers)
		b = &buffer;

	SetUniformBuffers(binding, buffers);
}

void IMaterialSlot::SetGpuImage(const std::string& binding, const IGpuImageView* image) {
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images{};

	for (auto& i : images)
		i = image;

	SetGpuImages(binding, images);
}

void IMaterialSlot::SetStorageBuffer(const std::string& binding, const GpuBuffer* buffer) {
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffers{};

	for (auto& b : buffers)
		b = buffer;

	SetStorageBuffers(binding, buffers);
}

void IMaterialSlot::SetStorageImage(const std::string& binding, const IGpuImageView* image) {
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images{};

	for (auto& i : images)
		i = image;

	SetStorageImages(binding, images);
}

void IMaterialSlot::SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure) {
	std::array<const ITopLevelAccelerationStructure*, NUM_RESOURCES_IN_FLIGHT> accelerationStructures{};

	for (auto& as : accelerationStructures)
		as = accelerationStructure;

	SetAccelerationStructures(binding, accelerationStructures);
}

std::string_view IMaterialSlot::GetName() const {
	return name;
}
