#include "RenderpassAttachment.h"

using namespace OSK::VULKAN;

void RenderpassAttachment::AddAttachment(VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout layout) {
	attahcmentDesc.format = format;
	//attahcmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	//LoadOp: qué se hace al renderizarsobre  una imagen.
	//	VK_ATTACHMENT_LOAD_OP_LOAD: se conserva lo que había antes.
	//	VK_ATTACHMENT_LOAD_OP_CLEAR: se limpia.
	//	VK_ATTACHMENT_LOAD_OP_DONT_CARE: da igual.
	attahcmentDesc.loadOp = loadOp;
	attahcmentDesc.storeOp = storeOp;
	//Stencil.
	attahcmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attahcmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attahcmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attahcmentDesc.finalLayout = layout;
}

void RenderpassAttachment::CreateReference(const uint32_t& attachment, VkImageLayout use) {
	reference.attachment = attachment;
	reference.layout = use;
}

