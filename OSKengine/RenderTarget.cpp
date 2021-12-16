#include "RenderTarget.h"

#include "VulkanImageGen.h"
#include "VulkanRenderer.h"

#include "MaterialSlot.h"

using namespace OSK;
using namespace OSK::VULKAN;


constexpr uint32_t defaultMipLevels = 1;

RenderTarget::RenderTarget(RenderAPI* renderer) {
	this->renderer = renderer;
}

RenderTarget::~RenderTarget() {
	Clear();
}

void RenderTarget::CreateRenderpass(VkSampleCountFlagBits msaa) {
	renderer->GetMaterialSystem()->UnregisterRenderpass(renderpass.GetPointer());

	msaa = renderer->GetMsaaSamples();

	renderpass.Delete();
	renderpass = renderer->CreateNewRenderpass().GetPointer();

	RenderpassAttachment colorAttachment;
	colorAttachment.AddAttachment(renderer->swapchain->GetFormat(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, msaa);
	colorAttachment.CreateReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	RenderpassAttachment depthAttachment{};
	depthAttachment.AddAttachment(renderer->getDepthFormat(), VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, msaa);
	depthAttachment.CreateReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	RenderpassAttachment finalColorAttachment{};
	if (swapchainViews == nullptr) {
		finalColorAttachment.AddAttachment(renderer->swapchain->GetFormat(), VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_SAMPLE_COUNT_1_BIT);
	}
	else {
		finalColorAttachment.AddAttachment(renderer->swapchain->GetFormat(), VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_SAMPLE_COUNT_1_BIT);
	}
	finalColorAttachment.CreateReference(2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	RenderpassSubpass subpass;
	subpass.SetPipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS);
	const std::vector<RenderpassAttachment> colorAttachments = { colorAttachment };
	subpass.SetColorAttachments(colorAttachments);
	subpass.SetDepthStencilAttachment(depthAttachment);
	subpass.SetResolveAttachment(finalColorAttachment);

	SubpassDependency dependency;
	dependency.vulkanDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.vulkanDependency.dstSubpass = 0;
	dependency.vulkanDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.vulkanDependency.srcAccessMask = 0;
	dependency.vulkanDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.vulkanDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	subpass.AddDependency(dependency);

	renderpass->AddAttachment(colorAttachment);
	renderpass->AddAttachment(depthAttachment);
	renderpass->AddAttachment(finalColorAttachment);
	renderpass->AddSubpass(subpass);
	renderpass->Create();

	renderer->GetMaterialSystem()->RegisterRenderpass(renderpass.GetPointer());

	CreateFramebuffers(renderer->swapchain->GetImageCount());
}

void RenderTarget::TransitionToRenderTarget(VkCommandBuffer cmdBuffer) {
	if (targetImage == RenderTargetImageTarget::COLOR)
		VULKAN::VulkanImageGen::TransitionImageLayout(renderedSprite.GetTexture()->image.GetPointer(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1, 1, cmdBuffer);
	else if (targetImage == RenderTargetImageTarget::DEPTH) {
		VULKAN::VulkanImageGen::TransitionImageLayout(&depthImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, 1, 1, cmdBuffer);
	}
}

void RenderTarget::TransitionToTexture(VkCommandBuffer cmdBuffer) {
	VULKAN::VulkanImageGen::TransitionImageLayout(renderedSprite.GetTexture()->image.GetPointer(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1, cmdBuffer);
}

void RenderTarget::CreateFramebuffers(uint32_t numFb) {
	for (uint32_t i = 0; i < numFb; i++) {
		framebuffers.push_back(renderer->CreateNewFramebuffer());
		
		framebuffers[i]->AddImageView(intermediateColorImage.view);
		framebuffers[i]->AddImageView(depthImage.view);

		if (swapchainViews == nullptr)
			framebuffers[i]->AddImageView(renderedSprite.GetTexture()->image->view);
		else
			framebuffers[i]->AddImageView(swapchainViews->at(i));

		framebuffers[i]->Create(renderpass.GetPointer(), size.X, size.Y);
	}
}

void RenderTarget::SetSize(uint32_t sizeX, uint32_t sizeY) {
	size.X = sizeX;
	size.Y = sizeY;

	if (spriteHasBeenCreated) {
		auto msaa = renderer->GetMsaaSamples();

		VULKAN::VulkanImageGen::CreateImage(&intermediateColorImage, size, renderer->swapchain->GetFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, defaultMipLevels, msaa);
		VULKAN::VulkanImageGen::CreateImageView(&intermediateColorImage, renderer->swapchain->GetFormat(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);

		VULKAN::VulkanImageGen::CreateImage(&depthImage, size, renderer->getDepthFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, defaultMipLevels, msaa);
		VULKAN::VulkanImageGen::CreateImageView(&depthImage, renderer->getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);

		VULKAN::VulkanImageGen::CreateImage(renderedSprite.GetTexture()->image.GetPointer(), size, renderer->swapchain->GetFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1, VK_SAMPLE_COUNT_1_BIT);
		VULKAN::VulkanImageGen::CreateImageView(renderedSprite.GetTexture()->image.GetPointer(), renderer->swapchain->GetFormat(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	}
	
	for (uint32_t i = 0; i < framebuffers.size(); i++) {
		framebuffers[i]->Clear();

		framebuffers[i]->attachments[0] = intermediateColorImage.view;
		framebuffers[i]->attachments[1] = depthImage.view;

		if (swapchainViews == nullptr)
			framebuffers[i]->attachments[2] = renderedSprite.GetTexture()->image->view;
		else
			framebuffers[i]->attachments[2] = swapchainViews->at(i);

		framebuffers[i]->Create(renderpass.GetPointer(), size.X, size.Y);
	}
}

void RenderTarget::Resize(uint32_t sizeX, uint32_t sizeY) {
	SetSize(sizeX, sizeY);
}

void RenderTarget::SetSwapchain(std::vector<VkImageView>& swapchainViews) {
	this->swapchainViews = &swapchainViews;
}

VULKAN::GpuImage* RenderTarget::GetDepthImage() {
	return &depthImage;
}

void RenderTarget::CreateSprite(ContentManager* content) {
	content->LoadSprite(&renderedSprite);
	renderedSprite.SetTexture(new Texture());
	this->content = content;
	renderedSprite.material = renderer->GetMaterialSystem()->GetMaterial(MPIPE_2D)->CreateInstance().GetPointer();

	auto msaa = renderer->GetMsaaSamples();

	VULKAN::VulkanImageGen::CreateImage(&intermediateColorImage, size, renderer->swapchain->GetFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, defaultMipLevels, msaa);
	VULKAN::VulkanImageGen::CreateImageView(&intermediateColorImage, renderer->swapchain->GetFormat(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(intermediateColorImage, SHADOW_MAP_FILTER, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);

	VULKAN::VulkanImageGen::CreateImage(&depthImage, size, renderer->getDepthFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, defaultMipLevels, msaa);
	VULKAN::VulkanImageGen::CreateImageView(&depthImage, renderer->getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(depthImage, SHADOW_MAP_FILTER, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);

	VULKAN::VulkanImageGen::CreateImage(renderedSprite.GetTexture()->image.GetPointer(), size, renderer->swapchain->GetFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1, VK_SAMPLE_COUNT_1_BIT);
	VULKAN::VulkanImageGen::CreateImageView(renderedSprite.GetTexture()->image.GetPointer(), renderer->swapchain->GetFormat(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(renderedSprite.GetTexture()->image.Get(), SHADOW_MAP_FILTER, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);

	spriteHasBeenCreated = true;

	renderedSprite.transform.SetScale(size.ToVector2f());

	renderedSprite.material->GetMaterialSlot(MSLOT_TEXTURE_2D)->SetTexture("Texture", renderedSprite.GetTexture());
	renderedSprite.material->FlushUpdate();
}

void RenderTarget::BeginRenderpass(VkCommandBuffer commandBuffer, uint32_t iteration) {
	VkRenderPassBeginInfo renderpassInfo{};
	renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpassInfo.renderPass = renderpass->vulkanRenderpass;
	renderpassInfo.framebuffer = framebuffers[iteration]->framebuffer;
	renderpassInfo.renderArea.offset = { 0,0 };
	renderpassInfo.renderArea.extent = { size.X, size.Y };

	std::array<VkClearValue, 2> clearColors;
	clearColors[0] = { clearColor.Red, clearColor.Green, clearColor.Blue, clearColor.Alpha };
	clearColors[1] = { 1.0f, 0.0f };

	renderpassInfo.clearValueCount = clearColors.size();
	renderpassInfo.pClearValues = clearColors.data();

	TransitionToRenderTarget(commandBuffer);

	vkCmdBeginRenderPass(commandBuffer, &renderpassInfo, VK_SUBPASS_CONTENTS_INLINE);

	lastCommandBuffer = commandBuffer;
}

void RenderTarget::EndRenderpass() {
	vkCmdEndRenderPass(lastCommandBuffer);
	TransitionToTexture(lastCommandBuffer);
}

void RenderTarget::Clear() {
	for (auto& i : framebuffers)
		i.Delete();

	delete renderedSprite.GetTexture();
	renderedSprite.SetTexture(nullptr);
}

Vector2ui RenderTarget::GetSize() {
	return size;
}
