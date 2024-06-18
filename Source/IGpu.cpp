#include "IGpu.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void IGpu::_SetName(const std::string& name) {
	m_name = name;
}

std::string_view IGpu::GetName() const {
	return m_name;
}

bool IGpu::SupportsRayTracing() const {
	return m_supportsRayTracing;
}

bool IGpu::SupportsBindlessResources() const{
	return m_supportsBindless;
}

const GpuMemoryAlignments& IGpu::GetMemoryAlignments() const {
	return m_minAlignments;
}

void IGpu::SetMinAlignments(const GpuMemoryAlignments& alignments) {
	m_minAlignments = alignments;
}
