#include "IGpu.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void IGpu::_SetName(const std::string& name) {
	m_name = name;
}

std::string_view IGpu::GetName() const {
	return m_name;
}
