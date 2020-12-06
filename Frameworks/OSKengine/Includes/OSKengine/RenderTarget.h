#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Framebuffer.h"
#include "Renderpass.h"
#include "VulkanImage.h"
#include "GraphicsPipeline.h"

#include "Sprite.h"
#include "ContentManager.h"
#include "Renderpass.h"

#include <vector>

namespace OSK {

	class RenderAPI;

	class OSKAPI_CALL RenderTarget {

		friend class RenderAPI;

	public:

		~RenderTarget();

		void SetFormat(VkFormat format);

		void CreateRenderpass(std::vector<VULKAN::RenderpassAttachment> colorAttachments, VULKAN::RenderpassAttachment* depthAttachment, VkSampleCountFlagBits msaa = VK_SAMPLE_COUNT_1_BIT);

		void CreateFramebuffers(const uint32_t& numFb, VkImageView* images, const uint32_t& numViews);

		void SetSize(const uint32_t& sizeX, const uint32_t& sizeY, const bool& createColorImage = true, const bool& updatePipelines = true);

		void CreateSprite(ContentManager* content);

		void TransitionToRenderTarget(VkCommandBuffer* cmdBuffer, VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		void TransitionToTexture(VkCommandBuffer* cmdBuffer);

		std::vector<VULKAN::Framebuffer*> TargetFramebuffers;
		VULKAN::Renderpass* VRenderpass = nullptr;
		
		std::vector<GraphicsPipeline*> Pipelines;

		void Clear(bool complete = true);

		Sprite RenderedSprite{};

		Vector2ui Size;

	private:

		RenderTarget(RenderAPI* renderer);

		ContentManager* Content = nullptr;
		RenderAPI* renderer = nullptr;

		bool spriteHasBeenCreated = false;
		VkFormat Format;

	};

}
