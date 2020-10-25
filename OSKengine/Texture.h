#pragma once

#include "VulkanImage.h"
#include <vector>

namespace OSK {

	class VulkanRenderer;
	class DescriptorSet;

	//Textura 2D.
	struct OSKAPI_CALL Texture {
		friend class VulkanRenderer;
		friend class Sprite;
		friend class ContentManager;

		uint32_t sizeX = 0;
		uint32_t sizeY = 0;

	private:
		VULKAN::VulkanImage Albedo{};

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

		VULKAN::VulkanImage Albedo{};
		VULKAN::VulkanImage Specular{};

		uint32_t sizeX = 0;
		uint32_t sizeY = 0;

		DescriptorSet* PhongDescriptorSet = nullptr;
		DescriptorSet* PBR_DescriptorSet = nullptr;

	private:
		ModelTexture(VkDevice logicalDevice);

		VkDevice logicalDevice;

	};


	struct OSKAPI_CALL SkyboxTexture {
		friend class VulkanRenderer;
		friend class ContentManager;
		friend class Skybox;

	private:
		VULKAN::VulkanImage texture{};
		
		DescriptorSet* Descriptor;
		
	};

}
