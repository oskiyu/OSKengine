#include "RenderpassVkMap.h"

#include "FormatVk.h"

#include "Assert.h"
#include "Exceptions.h"


using namespace OSK;
using namespace OSK::GRAPHICS;

bool RenderpassVkMapEntry::operator==(const RenderpassVkMapEntry& other) const {
	if (colors.GetSize() != other.colors.GetSize()) {
		return false;
	}

	for (USize64 i = 0; i < colors.GetSize(); i++) {
		if (colors[i] != other.colors[i]) {
			return false;
		}
	}

	if (depth.has_value() != other.depth.has_value()) {
		return false;
	}

	if (depth && *depth != *other.depth) {
		return false;
	}

	return true;
}

RenderpassVk* RenderpassVkMap::GetRenderpass(const GpuVk* gpu, const RenderpassVkMapEntry& entry) {
	if (auto it = m_map.find(entry); it != m_map.end()) {
		return &it->second;
	}

	auto attachments = DynamicArray<RenderpassAttachmentVk>::CreateReserved(entry.colors.GetSize() + 1);
	for (const auto& e : entry.colors) {
		OSK_ASSERT(e.format != Format::UNKNOWN, InvalidArgumentException("No se puede crear un renderpass sobre una imagen de color con formato UNKNOWN"));
		attachments.Insert(RenderpassAttachmentVk(
			GetFormatVk(e.format),
			e.clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
	}

	std::optional<RenderpassAttachmentVk> depthAttachment = std::nullopt;
	if (entry.depth) {
		OSK_ASSERT(entry.depth->format != Format::UNKNOWN, InvalidArgumentException("No se puede crear un renderpass sobre una imagen de profundidad con formato UNKNOWN"));
		depthAttachment = RenderpassAttachmentVk(
			GetFormatVk(entry.depth->format),
			entry.depth->clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	auto subpasses = DynamicArray<RenderSubpassVk>::CreateReserved(1);
	subpasses.Insert(RenderSubpassVk(VK_PIPELINE_BIND_POINT_GRAPHICS, attachments, depthAttachment));

	if (depthAttachment) {
		attachments.Insert(*depthAttachment);
	}

	return &m_map.try_emplace(entry, RenderpassVk(gpu, attachments, subpasses)).first->second;
}

void RenderpassVkMap::Clear() {
	m_map.clear();
}
