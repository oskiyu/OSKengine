#pragma once

#include "VulkanImage.h"
#include <vector>
#include "DescriptorSet.h"

namespace OSK {

	class VulkanRenderer;

	//Textura 2D.
	struct OSKAPI_CALL Texture {
		friend class VulkanRenderer;
		friend class Sprite;
		friend class ContentManager;

	private:
		VULKAN::VulkanImage Albedo{};

		uint32_t sizeX = 0;
		uint32_t sizeY = 0;

		DescriptorSet* Descriptor;
		DescriptorSet* PhongDescriptorSet = nullptr;
		DescriptorSet* PBR_DescriptorSet = nullptr;

	};


	//Textura para 3D.
	struct OSKAPI_CALL ModelTexture {
		friend class VulkanRenderer;
		friend class ContentManager;

	public:
		~ModelTexture();

	private:
		ModelTexture(VkDevice logicalDevice);

		VULKAN::VulkanImage Albedo{};
		VULKAN::VulkanImage Specular{};

		uint32_t sizeX = 0;
		uint32_t sizeY = 0;

		DescriptorSet* PhongDescriptorSet = nullptr;
		DescriptorSet* PBR_DescriptorSet = nullptr;

		VkDevice logicalDevice;

	};


	struct OSKAPI_CALL SkyboxTexture {
		friend class VulkanRenderer;
		friend class ContentManager;

	private:
		VULKAN::VulkanImage texture{};
		
		DescriptorSet* Descriptor;
		
	};

}
