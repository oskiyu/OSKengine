#include "RenderTarget.h"

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
	if (VRenderpass)
		delete VRenderpass;

	VRenderpass = renderer->CreateNewRenderpass();
	
	RenderpassSubpass sbPass{};
	sbPass.SetColorAttachments(colorAttachments);
	if (depthAttachment)
		sbPass.SetDepthStencilAttachment(*depthAttachment);
	sbPass.SetPipelineBindPoint();
	
	SubpassDependency dep;
	dep.VulkanDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dep.VulkanDependency.dstSubpass = 0;
	dep.VulkanDependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dep.VulkanDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dep.VulkanDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep.VulkanDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dep.VulkanDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	SubpassDependency dep2;
	dep2.VulkanDependency.srcSubpass = 0;
	dep2.VulkanDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	dep2.VulkanDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep2.VulkanDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dep2.VulkanDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dep2.VulkanDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dep2.VulkanDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	sbPass.AddDependency(dep);
	sbPass.AddDependency(dep2);

	VRenderpass->SetMSAA(msaa);
	VRenderpass->AddSubpass(sbPass);
	for (auto& i : colorAttachments)
		VRenderpass->AddAttachment(i);
	if (depthAttachment)
		VRenderpass->AddAttachment(*depthAttachment);

	VRenderpass->Create();

	renderer->GetMaterialSystem()->RegisterRenderpass(VRenderpass);
}

void RenderTarget::SetFormat(VkFormat format) {
	Format = format;
}

void RenderTarget::TransitionToRenderTarget(VkCommandBuffer* cmdBuffer, VkImageLayout layout) {
	VulkanImageGen::TransitionImageLayout(&RenderedSprite.Texture2D->Image, VK_IMAGE_LAYOUT_UNDEFINED, layout, 1, 1, cmdBuffer);
}

void RenderTarget::TransitionToTexture(VkCommandBuffer* cmdBuffer) {
	VulkanImageGen::TransitionImageLayout(&RenderedSprite.Texture2D->Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1, cmdBuffer);
}

void RenderTarget::CreateFramebuffers(uint32_t numFb, VkImageView* images, uint32_t numViews) {
	for (uint32_t i = 0; i < numFb; i++) {
		TargetFramebuffers.push_back(renderer->CreateNewFramebuffer());
		
		for (uint32_t v = 0; v < numViews; v++)
			TargetFramebuffers[i]->AddImageView(images[v]);

		TargetFramebuffers[i]->Create(VRenderpass, Size.X, Size.Y);
	}
}

void RenderTarget::SetSize(uint32_t sizeX, uint32_t sizeY, bool createColorImage, bool updatePipelines) {{}
	Size.X = sizeX;
	Size.Y = sizeY;

	if (spriteHasBeenCreated && createColorImage) {
		if (RenderedSprite.Texture2D->Image.Image != VK_NULL_HANDLE)
			RenderedSprite.Texture2D->Image.Destroy();

		VulkanImageGen::CreateImage(&RenderedSprite.Texture2D->Image, Size, Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
		VulkanImageGen::CreateImageView(&RenderedSprite.Texture2D->Image, Format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
		
		RenderedSprite.UpdateMaterialTexture();
	}

	if (updatePipelines) {
		for (auto& i : Pipelines) {
			i->~GraphicsPipeline();
			i->SetViewport({ 0, 0, (float)Size.X, (float)Size.Y });
			i->Create(VRenderpass);
		}
	}

	for (auto& i : TargetFramebuffers) {
		i->Clear();
		i->Create(VRenderpass, Size.X, Size.Y);
	}
}

void RenderTarget::CreateSprite(ContentManager* content) {
	content->CreateSprite(RenderedSprite);
	RenderedSprite.Texture2D = new Texture();
	Content = content;
	RenderedSprite.SpriteMaterial = renderer->GetMaterialSystem()->GetMaterial(renderer->DefaultMaterial2D_Name)->CreateInstance();

	spriteHasBeenCreated = true;
}

void RenderTarget::Clear(bool complete) {
	for (auto& i : TargetFramebuffers)
		delete i;
	TargetFramebuffers.clear();

	for (auto& i : Pipelines)
		delete i;
	Pipelines.clear();

	if (complete && VRenderpass)
		delete VRenderpass;
}