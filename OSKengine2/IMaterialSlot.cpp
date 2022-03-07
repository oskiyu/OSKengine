#include "IMaterialSlot.h"

#include "Texture.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IMaterialSlot::IMaterialSlot(const MaterialLayout* layout, const std::string& name)
	: name(name), layout(layout) {

}

void IMaterialSlot::SetTexture(const std::string& binding, const ASSETS::Texture* texture) {
	SetGpuImage(binding, texture->GetGpuImage());
}

std::string IMaterialSlot::GetName() const {
	return name;
}
