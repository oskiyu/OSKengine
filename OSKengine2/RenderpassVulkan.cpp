#include "RenderpassVulkan.h"

#include <vulkan/vulkan.h>
#include "SwapchainVulkan.h"
#include "IGpuImage.h"
#include "FormatVulkan.h"
#include "DynamicArray.hpp"
#include "OSKengine.h"
#include "RendererVulkan.h"
#include "GpuVulkan.h"
#include "GpuImageVulkan.h"
#include "IGpuMemoryAllocator.h"
#include "GpuMemoryTypes.h"
#include "Format.h"

using namespace OSK;

VkFormat GetSupportedFormat(const DynamicArray<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(
			OSK::Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetPhysicalDevice(), 
			format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
			return format;

		if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
			return format;
	}

	return candidates.At(0);
}

RenderpassVulkan::RenderpassVulkan(RenderpassType type) 
	: IRenderpass(type) {

}

RenderpassVulkan::~RenderpassVulkan() {
	if (renderpass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
			renderpass, nullptr);
		renderpass = VK_NULL_HANDLE;
	}

	for (auto i : framebuffers)
		vkDestroyFramebuffer(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
			i, nullptr);
}

void RenderpassVulkan::Create(const ISwapchain* swapchain) {
	Create(swapchain, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void RenderpassVulkan::CreateFinalPresent(const ISwapchain* swapchain) {
	Create(swapchain, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}

VkRenderPass RenderpassVulkan::GetRenderpass() const {
	return renderpass;
}

void RenderpassVulkan::Create(const ISwapchain* swapchain, VkImageLayout finalLayout) {
	swapchain->As<SwapchainVulkan>()->SetTargetRenderpass(this);
	auto msaa = (VkSampleCountFlagBits)Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetInfo().maxMsaaSamples;

	VkAttachmentDescription colorAttahcmentDesc{};
	VkAttachmentReference colorReference{};

	colorAttahcmentDesc.format = GetFormatVulkan(swapchain->GetImage(0)->GetFormat());
	colorAttahcmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttahcmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttahcmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttahcmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttahcmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttahcmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttahcmentDesc.samples = msaa;

	colorReference.attachment = 0;
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// ---------------------- //

	VkAttachmentDescription depthAttahcmentDesc{};
	VkAttachmentReference depthReference{};

	depthAttahcmentDesc.format = GetFormatVulkan(Format::D32S8_SFLOAT_SUINT);
	depthAttahcmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttahcmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	depthAttahcmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttahcmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttahcmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttahcmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttahcmentDesc.samples = msaa;

	depthReference.attachment = 1;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// ---------------------- //

	VkAttachmentDescription finalColorAttahcmentDesc{};
	VkAttachmentReference finalColorReference{};

	finalColorAttahcmentDesc.format = GetFormatVulkan(swapchain->GetImage(0)->GetFormat());
	finalColorAttahcmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	finalColorAttahcmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	finalColorAttahcmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	finalColorAttahcmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	finalColorAttahcmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	finalColorAttahcmentDesc.finalLayout = finalLayout;
	finalColorAttahcmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;

	finalColorReference.attachment = 2;
	finalColorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	// ------------- SUBPASSES ---------------- //

	VkSubpassDescription subpassDescription{};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorReference;
	subpassDescription.pDepthStencilAttachment = &depthReference;
	subpassDescription.pResolveAttachments = &finalColorReference;

	VkSubpassDependency subpassDependency{};
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


	// ---------- CREATE ----------- //

	VkAttachmentDescription tempAttachments[3] = {
		colorAttahcmentDesc,
		depthAttahcmentDesc,
		finalColorAttahcmentDesc
	};

	VkRenderPassCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = 3;
	createInfo.pAttachments = tempAttachments;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpassDescription;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &subpassDependency;

	VkResult result = vkCreateRenderPass(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
		&createInfo, nullptr, &renderpass);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el renderpass. Code: " + std::to_string(result));
}

void RenderpassVulkan::SetImage(GpuImage* image) {
	const auto size = image->GetSize();

	images[0] = image;
	images[1] = nullptr;

	VkFramebuffer framebuffer = VK_NULL_HANDLE;

	colorImgs[0] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
		size.X, size.Y, image->GetFormat(), 
		GpuImageUsage::COLOR, GpuSharedMemoryType::GPU_ONLY).GetPointer();

	depthImgs[0] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
		size.X, size.Y, Format::D32S8_SFLOAT_SUINT,
		GpuImageUsage::DEPTH_STENCIL, GpuSharedMemoryType::GPU_ONLY).GetPointer();

	VkImageView views[] = {
		colorImgs[0]->As<GpuImageVulkan>()->GetView(),
		depthImgs[0]->As<GpuImageVulkan>()->GetView(),
		image->As<GpuImageVulkan>()->GetView()
	};

	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderpass;
	framebufferInfo.attachmentCount = 3;
	framebufferInfo.pAttachments = views;
	framebufferInfo.width = image->GetSize().X;
	framebufferInfo.height = image->GetSize().Y;
	framebufferInfo.layers = 1;

	VkResult result = vkCreateFramebuffer(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
		&framebufferInfo, nullptr, &framebuffer);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el framebuffer. Code: " + std::to_string(result));

	framebuffers.Insert(framebuffer);
}

void RenderpassVulkan::SetImages(GpuImage* image0, GpuImage* image1, GpuImage* image2) {
	for (auto i : framebuffers)
		vkDestroyFramebuffer(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
			i, nullptr);
	framebuffers.Empty();

	const auto size = image0->GetSize();

	images[0] = image0;
	images[1] = image1;
	images[2] = image2;

	for (TSize i = 0; i < 3; i++) {
		colorImgs[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
			size.X, size.Y, image0->GetFormat(),
			GpuImageUsage::COLOR, GpuSharedMemoryType::GPU_ONLY).GetPointer();

		depthImgs[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
			size.X, size.Y, Format::D32S8_SFLOAT_SUINT,
			GpuImageUsage::DEPTH_STENCIL, GpuSharedMemoryType::GPU_ONLY).GetPointer();
	}

	VkImageView finalViews[] = {
		image0->As<GpuImageVulkan>()->GetView(),
		image1->As<GpuImageVulkan>()->GetView(),
		image2->As<GpuImageVulkan>()->GetView()
	};

	for (TSize i = 0; i < GetNumberOfImages(); i++) {
		VkFramebuffer framebuffer = VK_NULL_HANDLE;

		VkImageView imgs[] = {
			colorImgs[i]->As<GpuImageVulkan>()->GetView(),
			depthImgs[i]->As<GpuImageVulkan>()->GetView(),
			finalViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass;
		framebufferInfo.attachmentCount = 3;
		framebufferInfo.pAttachments = imgs;
		framebufferInfo.width = image0->GetSize().X;
		framebufferInfo.height = image0->GetSize().Y;
		framebufferInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
			&framebufferInfo, nullptr, &framebuffer);
		OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear el framebuffer. Code: " + std::to_string(result));

		framebuffers.Insert(framebuffer);
	}
}

VkFramebuffer RenderpassVulkan::GetFramebuffer(TSize index) const {
	OSK_ASSERT(index >= 0 && index < GetNumberOfImages(), "Se ha intentado acceder al framebuffer "
		+ std::to_string(index) + ", pero solo hay " + std::to_string(GetNumberOfImages()) + " framebuffers.");

	return framebuffers[index];
}
