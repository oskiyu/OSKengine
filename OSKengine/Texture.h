#pragma once

#include "VulkanImage.h"
#include <vector>
#include "DescriptorSet.h"

namespace OSK {

	class VulkanRenderer;

	//Textura.
	struct OSKAPI_CALL Texture {
		friend class VulkanRenderer;
		friend class Sprite;

	private:
		VULKAN::VulkanImage image{};

		uint32_t sizeX = 0;
		uint32_t sizeY = 0;

		DescriptorSet* Descriptor;

	};

}
