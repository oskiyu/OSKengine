#include "IMaterialSlot.h"

#include "Texture.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IMaterialSlot::IMaterialSlot(const MaterialLayout* layout, const std::string& name)
	: layout(layout), name(name) {

}

void IMaterialSlot::SetTexture(const std::string& binding, const ASSETS::Texture* texture, SampledChannel channel) {
	SetGpuImage(binding, texture->GetGpuImage(), channel);
}

void IMaterialSlot::SetTextures(const std::string& binding, const ASSETS::Texture* texture[NUM_RESOURCES_IN_FLIGHT], SampledChannel channel) {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = texture[i]->GetGpuImage();

	SetGpuImages(binding, images, channel);
}

std::string IMaterialSlot::GetName() const {
	return name;
}
