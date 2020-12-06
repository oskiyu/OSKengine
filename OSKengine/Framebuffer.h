#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include "Renderpass.h"
#include "VulkanImage.h"

namespace OSK {

	class RenderAPI;

}

namespace OSK::VULKAN {

	class OSKAPI_CALL Framebuffer {

		friend class OSK::RenderAPI;

	public:

		~Framebuffer();

		void AddImageView(VULKAN::VulkanImage* image);
		void AddImageView(VkImageView view);

		void Create(VULKAN::Renderpass* renderpass, const uint32_t& sizeX, const uint32_t& sizeY);

		void Clear();

		VkFramebuffer framebuffer;

	private:

		Framebuffer(OSK::RenderAPI* renderer);

		OSK::RenderAPI* renderer = nullptr;

		std::vector<VkImageView> attachments;

		Vector2ui size;

		bool isActive = false;

	};

}