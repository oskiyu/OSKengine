#pragma once

#include "IGpuImage.h"
#include "DynamicArray.hpp"
#include "GpuImageSamplerDesc.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {
	
	class OSKAPI_CALL GpuImageVulkan : public GpuImage {

	public:

		GpuImageVulkan(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format, TSize numSamples, GpuImageSamplerDesc samplerDesc);
		~GpuImageVulkan();

		/* VULKAN SPECIFICS */

		/// <summary>
		/// Crea la imagen con los parámetros pasados por el constructor.
		/// </summary>
		/// 
		/// @throws std::runtime_exception si no se puede crear la imagen en la GPU.
		void CreateVkImage();

		/// <summary>
		/// Devuelve la imagen nativa.
		/// </summary>
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// 
		/// @note Si no se cumple la precondición, se devuelve VK_NULL_HANDLE.
		VkImage GetVkImage() const;


		/// <summary>
		/// Crea image views que representan el aspecto de color de la imagen.
		/// </summary>
		/// 
		/// @note Se crea un view por cada nivel del array.
		/// @note Si no es un array, sólo se crea un view en el nivel 0.
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::COLOR.
		void CreateColorViews();

		/// <summary>
		/// Devuelve el image view que representan el aspecto de color de la imagen.
		/// </summary>
		/// <param name="arrayLevel">Nivel en el array de la imagen.</param>
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::COLOR.
		/// @pre arrayLevel debe ser menor o igual al tamaño del array de la imagen.
		VkImageView GetColorView(TSize arrayLevel = 0) const;

		/// <summary>
		/// Crea un image view que representa el aspecto de color de la imagen.
		/// Para ser usado en un shader como sampler2Darray.
		/// </summary>
		/// 
		/// @note Se crea un view con todos los niveles del array.
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::COLOR.
		void CreateColorArrayView();

		/// <summary>
		/// Devuelve el image view que representa el aspecto de color de la imagen.
		/// Para ser usado en un shader como sampler2Darray.
		/// </summary>
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::COLOR.
		VkImageView GetColorArrayView() const;


		/// <summary>
		/// Crea image views que representan el aspecto de stencil y profundiad de la imagen.
		/// Para ser usado con depth-stencil en renderizado.
		/// </summary>
		/// 
		/// @note Se crea un view por cada nivel del array.
		/// @note Si no es un array, sólo se crea un view en el nivel 0.
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::DEPTH_STENCIL.
		void CreateDepthStencilViews();

		/// <summary>
		/// Devuelve el image view que representan los aspectos de stencil y profundiad de la imagen.
		/// Para ser usado con depth-stencil en renderizado.
		/// </summary>
		/// <param name="arrayLevel">Nivel en el array de la imagen.</param>
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::DEPTH_STENCIL.
		/// @pre arrayLevel debe ser menor o igual al tamaño del array de la imagen.
		VkImageView GetDepthStencilView(TSize arrayLevel = 0) const;


		/// <summary>
		/// Crea image views que representan el aspecto de profundiad de la imagen.
		/// </summary>
		/// 
		/// @note Se crea un view por cada nivel del array.
		/// @note Si no es un array, sólo se crea un view en el nivel 0.
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::DEPTH_STENCIL.
		void CreateDepthOnlyViews();

		/// <summary>
		/// Devuelve el image view que representan el aspecto de profundiad de la imagen.
		/// </summary>
		/// <param name="arrayLevel">Nivel en el array de la imagen.</param>
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::DEPTH_STENCIL.
		/// @pre arrayLevel debe ser menor o igual al tamaño del array de la imagen.
		VkImageView GetDepthOnlyView(TSize arrayLevel = 0) const;
		
		/// <summary>
		/// Crea un image view que representa el aspecto de profundidad de la imagen.
		/// Para ser usado en un shader como sampler2Darray.
		/// </summary>
		/// 
		/// @note Se crea un view con todos los niveles del array.
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::DEPTH_STENCIL.
		void CreateDepthArrayView();

		/// <summary>
		/// Devuelve el image view que representa el aspecto de profundidad de la imagen.
		/// Para ser usado en un shader como sampler2Darray.
		/// </summary>
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::DEPTH_STENCIL.
		VkImageView GetDepthArrayView() const;


		/// <summary>
		/// Crea image views que representan el aspecto de stencil de la imagen.
		/// </summary>
		/// 
		/// @note Se crea un view por cada nivel del array.
		/// @note Si no es un array, sólo se crea un view en el nivel 0.
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::DEPTH_STENCIL.
		void CreateStencilOnlyViews();

		/// <summary>
		/// Devuelve el image view que representan el aspecto de profundiad de la imagen.
		/// </summary>
		/// <param name="arrayLevel">Nivel en el array de la imagen.</param>
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::DEPTH_STENCIL.
		/// @pre arrayLevel debe ser menor o igual al tamaño del array de la imagen.
		VkImageView GetStencilOnlyView(TSize arrayLevel = 0) const;

		/// <summary>
		/// Crea un image view que representa el aspecto de stencil de la imagen.
		/// Para ser usado en un shader como sampler2Darray.
		/// </summary>
		/// 
		/// @note Se crea un view con todos los niveles del array.
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::DEPTH_STENCIL.
		void CreateStencilArrayView();

		/// <summary>
		/// Devuelve el image view que representa el aspecto de stencil de la imagen.
		/// Para ser usado en un shader como sampler2Darray.
		/// </summary>
		/// 
		/// @pre Debe haberse creado la imagen con GpuImageVulkan::CreateVkImage.
		/// @pre Debe haberse creado la imagen con GpuImageUsage::DEPTH_STENCIL.
		VkImageView GetStencilArrayView() const;


		/// <summary>
		/// Crea el sampler para la imagen.
		/// </summary>
		void CreateVkSampler(const GpuImageSamplerDesc& sampler);

		/// <summary>
		/// Devuelve el sampler de la imagen.
		/// </summary>
		VkSampler GetVkSampler() const;

		/* SWAPCHAIN */
		void _SetVkImage(VkImage img);
		void _SetView(VkImageView view);

	private:

		VkImageType GetVkImageType() const;
		VkImageViewType GetVkImageViewType() const;
		VkImageViewType GetVkImageArrayViewType() const;

		static VkFilter GetFilterTypeVulkan(GpuImageFilteringType type);
		static VkSamplerAddressMode GetAddressModeVulkan(GpuImageAddressMode mode);

		VkImage image = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;

		VkImageView arrayColorView = VK_NULL_HANDLE;
		VkImageView arrayDepthView = VK_NULL_HANDLE;
		VkImageView arrayStencilView = VK_NULL_HANDLE;

		DynamicArray<VkImageView> colorViews;
		DynamicArray<VkImageView> depthStencilViews;
		DynamicArray<VkImageView> depthViews;
		DynamicArray<VkImageView> stencilViews;

	};

}
