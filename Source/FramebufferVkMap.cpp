#include "FramebufferVkMap.h"
#include "NumericTypes.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


bool FramebufferVkMapEntry::operator==(const FramebufferVkMapEntry& other) const {
	if (views.GetSize() != other.views.GetSize()) {
		return false;
	}

	for (USize64 i = 0; i < views.GetSize(); i++) {
		if (views[i] != other.views[i]) {
			return false;
		}
	}

	return true;
}

FramebufferVkMap::FramebufferVkMap(const GpuVk* gpu) : m_gpu(gpu) {

}

FramebufferVk* FramebufferVkMap::GetFramebuffer(const FramebufferVkMapEntry& entry, const RenderpassVk& renderpass) {
	if (auto it = m_map.find(entry); it != m_map.end()) {
		return &it->second;
	}

	return &m_map.try_emplace(entry, FramebufferVk(*m_gpu, renderpass, entry.views.GetFullSpan())).first->second;
}
