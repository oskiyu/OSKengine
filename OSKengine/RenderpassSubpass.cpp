#include "RenderpassSubpass.h"

using namespace OSK::VULKAN;

RenderpassSubpass::~RenderpassSubpass() {
	references.clear();
}

void RenderpassSubpass::SetPipelineBindPoint(VkPipelineBindPoint point) {
	description.pipelineBindPoint = point;
}

void RenderpassSubpass::SetColorAttachments(const std::vector<RenderpassAttachment>& attachments) {
	description.colorAttachmentCount = attachments.size();

	for (uint32_t i = 0; i < attachments.size(); i++) {
		references.push_back(attachments[i].reference);
	}
	description.pColorAttachments = references.data();

}

void RenderpassSubpass::SetDepthStencilAttachment(const RenderpassAttachment& attachment) {
	description.pDepthStencilAttachment = &attachment.reference;
}

void RenderpassSubpass::AddDependency(SubpassDependency dependency) {
	dependencies.push_back(dependency);
}
