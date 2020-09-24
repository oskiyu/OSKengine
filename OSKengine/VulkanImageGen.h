#pragma once

#include <vulkan/vulkan.h>
#include "VulkanImage.h"
#include "Vector2.h"
#include "VulkanBuffer.h"

namespace OSK {

	class VulkanRenderer;

}

namespace OSK::VULKAN {

	class OSKAPI_CALL VulkanImageGen {

	public:

		//Establece el renderizador que usará esta clase.
		//	<renderer>: renderizador.
		static void SetRenderer(VulkanRenderer* renderer);

		//Crea una imagen de Vulkan.
		//	<image>: imagen.
		//	<size>: tamaño de la imagen.
		//	<format>: formato de la imagen.
		//	<tiling>: tiling de la imagen.
		//	<usage>: uso que se le va a dar a la imagen.
		//	<properties>: propiedaes de la memoria en la que se almacenará la imagen.
		//	<arrayLevels>: niveles de array de la imagen (para arays de imágenes).
		//	<flags> : 0.
		//	<mipLevels>: mipLevels.
		static void CreateImage(VULKAN::VulkanImage* image, const Vector2ui& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, const uint32_t& arrayLevels, VkImageCreateFlagBits flags, const uint32_t& mipLevels);

		//Crea un image view (cómo se accede a la imagen).
		static void CreateImageView(VULKAN::VulkanImage* img, VkFormat format, VkImageAspectFlags aspect, VkImageViewType type, const uint32_t& layerCount, const uint32_t& mipLevels);

		//Crea el image sampler de una imagen.
		//	<image>: imagen.
		//	<filter>: filtro (LINEAR: suave / NEAREST: pixelado).
		//	<addressMode>: ¿qué pasas si accedemos a TexCoords fuera de (-1, 1)?
		//	<mipLevels>: niveles del mipmap.
		static void CreateImageSampler(VulkanImage& image, VkFilter filter, VkSamplerAddressMode addressMode, const uint32_t& mipLevels);

		//Crea mipmaps para una imagen.
		//	<image>: imagen.
		//	<size>: tamaño de la imagen original (uint32_t).
		//	<mipLevels>: niveles del mipmap.
		static void CreateMipmaps(VulkanImage& image, const Vector2ui& size, const uint32_t& levels);

		static void CopyBufferToImage(VulkanBuffer* buffer, VULKAN::VulkanImage* img, const uint32_t& width, const uint32_t& height);

		static void TransitionImageLayout(VulkanImage* img, VkImageLayout oldLayout, VkImageLayout newLayout, const uint32_t& mipLevels, const uint32_t& arrayLevels);

		static VulkanImage CreateImageFromBitMap(uint32_t width, uint32_t height, uint8_t* pixels);


	private:

		static VulkanRenderer* renderer;

	};

}
