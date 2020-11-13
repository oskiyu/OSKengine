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

#include <vector>

namespace OSK {

	class RenderTarget {

	public:

		void CreateSprite(ContentManager* content);

		std::vector<VULKAN::Framebuffer*> TargetFramebuffers;
		VULKAN::Renderpass* VRenderpass = nullptr;
		
		std::vector<GraphicsPipeline*> Pipelines;

		std::vector<VULKAN::VulkanImage*> images;

		Sprite RenderedSprite{};

	private:

	};

}
