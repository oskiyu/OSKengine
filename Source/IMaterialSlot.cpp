#include "IMaterialSlot.h"

#include "OSKengine.h"
#include "IRenderer.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IMaterialSlot::IMaterialSlot(const MaterialLayout* layout, const std::string& name)
	: m_layout(layout), m_name(name) {

}

void IMaterialSlot::SetGpuImage(std::string_view binding, const IGpuImageView& image, const GpuImageSamplerDesc& samplerInfo, UIndex32 arrayIndex) {
	SetGpuImage(
		binding,
		image,
		Engine::GetRenderer()->GetSampler(samplerInfo),
		arrayIndex);
}

std::string_view IMaterialSlot::GetName() const {
	return m_name;
}

const MaterialLayout* IMaterialSlot::GetLayout() const {
	return m_layout;
}
