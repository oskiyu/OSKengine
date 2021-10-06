#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>
#include "GpuImage.h"
#include "GpuDataBuffer.h"
#include "SharedPtr.hpp"
#include "Texture.h"

namespace OSK {

	class RenderAPI;

}

namespace OSK::VULKAN {

	/// <summary>
	/// Clase estática para el manejo de imágenes de Vulkan.
	/// </summary>
	class OSKAPI_CALL VulkanImageGen {

	public:

		/// <summary>
		/// Establece el renderizador que usará esta clase.
		/// </summary>
		/// <param name="renderer">Renderizador.</param>
		static void SetRenderer(RenderAPI* renderer);

		/// <summary>
		/// Crea una imagen de Vulkan.
		/// </summary>
		/// <param name="image">Imagen a crear.</param>
		/// <param name="size">Tamaño de la imagen.</param>
		/// <param name="format">Formato de la imagen.</param>
		/// <param name="tiling">Tiling de la imagen.</param>
		/// <param name="usage">Uso de la imagen.</param>
		/// <param name="properties">Propiedades da la imagen.</param>
		/// <param name="arrayLevels">Array levels (1 por defecto).</param>
		/// <param name="flags">Otras flags.</param>
		/// <param name="mipLevels">Mipmaps (0 por defecto).</param>
		static void CreateImage(VULKAN::GpuImage* image, const Vector2ui& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t arrayLevels, VkImageCreateFlagBits flags, uint32_t mipLevels, VkSampleCountFlagBits msaaLevel);

		/// <summary>
		/// Crea un image view (cómo se accede a la imagen).
		/// </summary>
		/// <param name="img">Imagen con el View.</param>
		/// <param name="format">Formato.</param>
		/// <param name="aspect">Aspecto.</param>
		/// <param name="type">Tipo de view.</param>
		/// <param name="layerCount">Número de capas.</param>
		/// <param name="mipLevels">Niveles de mipmaps.</param>
		static void CreateImageView(VULKAN::GpuImage* img, VkFormat format, VkImageAspectFlags aspect, VkImageViewType type, uint32_t layerCount, uint32_t mipLevels);

		/// <summary>
		/// Crea un image view (cómo se accede a la imagen).
		/// </summary>
		/// <param name="view">View a crear.</param>
		/// <param name="image">Imagen del view.</param>
		/// <param name="format">Formato.</param>
		/// <param name="aspect">Aspecto.</param>
		/// <param name="type">Tipo de view.</param>
		/// <param name="layerCount">Número de capas.</param>
		/// <param name="mipLevels">Niveles de mipmaps.</param>
		static void CreateImageView(VkImageView* view, VkImage* image, VkFormat format, VkImageAspectFlags aspect, VkImageViewType type, uint32_t layerCount, uint32_t mipLevels);

		/// <summary>
		/// Crea el image sampler de una imagen.
		/// </summary>
		/// <param name="image">Imagen owner del sampler.</param>
		/// <param name="filter">Filtro (LINEAR: suave / NEAREST: pixelado).</param>
		/// <param name="addressMode">Qué hacer cuando las coordenadas están fuera de rango.</param>
		/// <param name="mipLevels">Niveles de mipmaps.</param>
		static void CreateImageSampler(GpuImage& image, VkFilter filter, VkSamplerAddressMode addressMode, uint32_t mipLevels);

		/// <summary>
		/// Crea mipmaps para una imagen.
		/// </summary>
		/// <param name="image">Imagen (a modificar).</param>
		/// <param name="size">Tamaño de la imagen original (uint32_t).</param>
		/// <param name="levels">Niveles a crear.</param>
		static void CreateMipmaps(GpuImage& image, const Vector2ui& size, uint32_t levels);

		/// <summary>
		/// Copia el contenido de un buffer a una imagen.
		/// </summary>
		/// <param name="buffer">Buffer que contiene la imagen.</param>
		/// <param name="img">Imagen.</param>
		/// <param name="width">Ancho de la imagen.</param>
		/// <param name="height">Alto de la imagen.</param>
		static void CopyBufferToImage(GpuDataBuffer* buffer, VULKAN::GpuImage* img, uint32_t width, uint32_t height);

		/// <summary>
		/// Cambia el layout de una imagen.
		/// </summary>
		/// <param name="img">Imagen.</param>
		/// <param name="oldLayout">Antiguo layout.</param>
		/// <param name="newLayout">Nuevo layout.</param>
		/// <param name="mipLevels">Niveles de mipmaps.</param>
		/// <param name="arrayLevels">Niveles de arrays (1 por defecto).</param>
		/// <param name="cmdBuffer">Command buffer a usar. SI es nullptr (por defecto), usará un command buffer de un solo uso.</param>
		static void TransitionImageLayout(GpuImage* img, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t arrayLevels, VkCommandBuffer cmdBuffer = VK_NULL_HANDLE);

		/// <summary>
		/// Crea una imagen a partir de un bitmap.
		/// </summary>
		/// <param name="width">Ancho del bitmap.</param>
		/// <param name="height">Alto del bitmap.</param>
		/// <param name="pixels">Bitmap.</param>
		/// <returns>Imagen a partir del bitmap.</returns>
		static SharedPtr<GpuImage> CreateImageFromBitMap(const Bitmap& bitmap);


	private:

		/// <summary>
		/// Renderizador.
		/// </summary>
		static RenderAPI* renderer;

	};

}
