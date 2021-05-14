#include "RenderTarget.h"

#include "VulkanImageGen.h"
#include "VulkanRenderer.h"

using namespace OSK;
using namespace OSK::VULKAN;


RenderTarget::RenderTarget(RenderAPI* renderer) {
	this->renderer = renderer;
}

RenderTarget::~RenderTarget() {
	Clear(true);
}

void RenderTarget::CreateRenderpass(std::vector<RenderpassAttachment> colorAttachments, RenderpassAttachment* depthAttachment, VkSampleCountFlagBits msaa) {
	if (vulkanRenderpass)
		delete vulkanRenderpass;

	vulkanRenderpass = renderer->CreateNewRenderpass();
	
	RenderpassSubpass sbPass{};
	sbPass.SetColorAttachments(colorAttachments);
	if (depthAttachment)
		sbPass.SetDepthStencilAttachment(*depthAttachment);
	sbPass.SetPipelineBindPoint();
	
	SubpassDependency dep;
	dep.vulkanDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dep.vulkanDependency.dstSubpass = 0;
	dep.vulkanDependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dep.vulkanDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dep.vulkanDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep.vulkanDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dep.vulkanDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	SubpassDependency dep2;
	dep2.vulkanDependency.srcSubpass = 0;
	dep2.vulkanDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	dep2.vulkanDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep2.vulkanDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dep2.vulkanDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dep2.vulkanDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dep2.vulkanDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	sbPass.AddDependency(dep);
	sbPass.AddDependency(dep2);

	vulkanRenderpass->SetMSAA(msaa);
	vulkanRenderpass->AddSubpass(sbPass);
	for (auto& i : colorAttachments)
		vulkanRenderpass->AddAttachment(i);
	if (depthAttachment)
		vulkanRenderpass->AddAttachment(*depthAttachment);

	vulkanRenderpass->Create();

	renderer->GetMaterialSystem()->RegisterRenderpass(vulkanRenderpass);
}

void RenderTarget::SetFormat(VkFormat format) {
	this->format = format;
}

void RenderTarget::TransitionToRenderTarget(VkCommandBuffer* cmdBuffer, VkImageLayout layout) {
	VULKAN::VulkanImageGen::TransitionImageLayout(&renderedSprite.texture->image, VK_IMAGE_LAYOUT_UNDEFINED, layout, 1, 1, cmdBuffer);
}

void RenderTarget::TransitionToTexture(VkCommandBuffer* cmdBuffer) {
	VULKAN::VulkanImageGen::TransitionImageLayout(&renderedSprite.texture->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1, cmdBuffer);
}

void RenderTarget::CreateFramebuffers(uint32_t numFb, VkImageView* images, uint32_t numViews) {
	for (uint32_t i = 0; i < numFb; i++) {
		targetFramebuffers.push_back(renderer->CreateNewFramebuffer());
		
		for (uint32_t v = 0; v < numViews; v++)
			targetFramebuffers[i]->AddImageView(images[v]);

		targetFramebuffers[i]->Create(vulkanRenderpass, size.X, size.Y);
	}
}

void RenderTarget::SetSize(uint32_t sizeX, uint32_t sizeY, bool createColorImage) {
	size.X = sizeX;
	size.Y = sizeY;

	if (spriteHasBeenCreated && createColorImage) {
		if (renderedSprite.texture->image.image != VK_NULL_HANDLE)
			renderedSprite.texture->image.Destroy();

		VULKAN::VulkanImageGen::CreateImage(&renderedSprite.texture->image, size, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
		VULKAN::VulkanImageGen::CreateImageView(&renderedSprite.texture->image, format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
		
		renderedSprite.UpdateMaterialTexture();
	}
	
	for (auto& i : targetFramebuffers) {
		i->Clear();
		i->Create(vulkanRenderpass, size.X, size.Y);
	}
}

void RenderTarget::CreateSprite(ContentManager* content) {
	content->CreateSprite(renderedSprite);
	renderedSprite.texture = new Texture();
	this->content = content;
	renderedSprite.material = renderer->GetMaterialSystem()->GetMaterial(renderer->defaultMaterial2D_Name)->CreateInstance();

	spriteHasBeenCreated = true;
}

void RenderTarget::Clear(bool complete) {
	for (auto& i : targetFramebuffers)
		delete i;
	targetFramebuffers.clear();

	if (complete && vulkanRenderpass)
		delete vulkanRenderpass;
}

Vector2ui RenderTarget::GetSize() {
	return size;
}