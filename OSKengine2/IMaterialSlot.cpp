#include "IMaterialSlot.h"

#include "Texture.h"
#include <array>

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IMaterialSlot::IMaterialSlot(const MaterialLayout* layout, const std::string& name)
	: m_layout(layout), m_name(name) {

}

void IMaterialSlot::SetTexture(const std::string& binding, const Texture* texture, SampledChannel channel, UIndex32 arrayIndex) {
	SetGpuImage(binding, texture->GetGpuImage()->GetView(GpuImageViewConfig::CreateSampled_Default()), arrayIndex);
}

void IMaterialSlot::SetTextures(const std::string& binding, std::span<const Texture*, NUM_RESOURCES_IN_FLIGHT> textures, SampledChannel channel, UIndex32 arrayIndex) {
	auto viewConfig = GpuImageViewConfig::CreateSampled_Default();
	viewConfig.channel = channel;

	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = textures[i]->GetGpuImage()->GetView(viewConfig);

	SetGpuImages(binding, images, arrayIndex);
}

void IMaterialSlot::SetUniformBuffer(const std::string& binding, const GpuBuffer& buffer, UIndex32 arrayIndex) {
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffers{};

	for (auto& b : buffers)
		b = &buffer;

	SetUniformBuffers(binding, buffers, arrayIndex);
}

void IMaterialSlot::SetGpuImage(const std::string& binding, const IGpuImageView* image, UIndex32 arrayIndex) {
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images{};

	for (auto& i : images)
		i = image;

	SetGpuImages(binding, images, arrayIndex);
}

void IMaterialSlot::SetStorageBuffer(const std::string& binding, const GpuBuffer* buffer, UIndex32 arrayIndex) {
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffers{};

	for (auto& b : buffers)
		b = buffer;

	SetStorageBuffers(binding, buffers, arrayIndex);
}

void IMaterialSlot::SetStorageImage(const std::string& binding, const IGpuImageView* image, UIndex32 arrayIndex) {
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images{};

	for (auto& i : images)
		i = image;

	SetStorageImages(binding, images, arrayIndex);
}

void IMaterialSlot::SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure, UIndex32 arrayIndex) {
	std::array<const ITopLevelAccelerationStructure*, NUM_RESOURCES_IN_FLIGHT> accelerationStructures{};

	for (auto& as : accelerationStructures)
		as = accelerationStructure;

	SetAccelerationStructures(binding, accelerationStructures, arrayIndex);
}

std::string_view IMaterialSlot::GetName() const {
	return m_name;
}
