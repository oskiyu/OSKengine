#pragma once

#include "VulkanImage.h"
#include <vector>

namespace OSK {

	class RenderAPI;
	class DescriptorSet;

	//Textura 2D.
	struct OSKAPI_CALL Texture {
		friend class RenderAPI;
		friend class Sprite;
		friend class ContentManager;
		friend class RenderTarget;

		uint32_t sizeX = 0;
		uint32_t sizeY = 0;

		VULKAN::VulkanImage Albedo{};

	private:
		DescriptorSet* Descriptor;
		DescriptorSet* PhongDescriptorSet = nullptr;
		DescriptorSet* PBR_DescriptorSet = nullptr;

	};


	//Textura para 3D.
	struct OSKAPI_CALL ModelTexture {
		friend class RenderAPI;
		friend class ContentManager;

	public:
		~ModelTexture();

		VULKAN::VulkanImage Albedo{};
		VULKAN::VulkanImage Specular{};

		uint32_t sizeX = 0;
		uint32_t sizeY = 0;

		DescriptorSet* PhongDescriptorSet = nullptr;
		DescriptorSet* DirShadowsDescriptorSet = nullptr;
		DescriptorSet* PointShadowsDescriptorSet = nullptr;
		DescriptorSet* PBR_DescriptorSet = nullptr;

	private:
		ModelTexture(VkDevice logicalDevice);

		VkDevice logicalDevice;

	};


	struct OSKAPI_CALL SkyboxTexture {
		friend class RenderAPI;
		friend class ContentManager;
		friend class Skybox;

	private:
		VULKAN::VulkanImage texture{};
		
		DescriptorSet* Descriptor;
		
	};

}
