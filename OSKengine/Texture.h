#pragma once

#include "VulkanImage.h"
#include <vector>

namespace OSK {

	struct OSKAPI_CALL Texture {

		~Texture() {
			Image.Destroy();
		}

		Vector2ui Size;
		VULKAN::VulkanImage Image;
	};

}
