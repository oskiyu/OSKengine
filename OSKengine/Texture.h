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
		VULKAN::VulkanImage Albedo{};

		uint32_t sizeX = 0;
		uint32_t sizeY = 0;

		DescriptorSet* Descriptor;
		DescriptorSet* PhongDescriptorSet = nullptr;
		DescriptorSet* PBR_DescriptorSet = nullptr;

	};


	struct OSKAPI_CALL ModelTexture {
		friend class VulkanRenderer;

	private:
		VULKAN::VulkanImage Albedo{};
		VULKAN::VulkanImage Specular{};

		uint32_t sizeX = 0;
		uint32_t sizeY = 0;

		DescriptorSet* PhongDescriptorSet = nullptr;
		DescriptorSet* PBR_DescriptorSet = nullptr;

	};

}
