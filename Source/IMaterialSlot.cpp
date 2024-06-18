#include "IMaterialSlot.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IMaterialSlot::IMaterialSlot(const MaterialLayout* layout, const std::string& name)
	: m_layout(layout), m_name(name) {

}

std::string_view IMaterialSlot::GetName() const {
	return m_name;
}

const MaterialLayout* IMaterialSlot::GetLayout() const {
	return m_layout;
}
