#pragma once

#include "IGpuImage.h"
#include "DynamicArray.hpp"
#include "GpuImageSamplerDesc.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {
	
	class OSKAPI_CALL GpuImageVk final : public GpuImage {

	public:

		GpuImageVk(
			const Vector3ui& size, 
			GpuImageDimension dimension, 
			GpuImageUsage usage, 
			USize32 numLayers, 
			Format format, 
			USize32 numSamples,
			GpuImageSamplerDesc samplerDesc);

		~GpuImageVk();

		OSK_DISABLE_COPY(GpuImageVk);

		/// <summary>
		/// Crea la imagen con los parámetros pasados por el constructor.
		/// </summary>
		/// 
		/// @throws ImageCreationException si no se puede crear la imagen en la GPU.
		void CreateVkImage();
		VkImage GetVkImage() const;

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
		
		void SetSwapchainView(VkImageView view);
		VkImageView GetSwapchainView() const;

		void SetDebugName(const std::string& name) override;

	protected:

		OwnedPtr<IGpuImageView> CreateView(const GpuImageViewConfig& viewConfig) const override;

	private:

		VkImageType GetVkImageType() const;
		VkImageViewType GetVkImageViewType() const;
		VkImageViewType GetVkImageArrayViewType() const;

		static VkFilter GetFilterTypeVulkan(GpuImageFilteringType type);
		static VkSamplerAddressMode GetAddressModeVulkan(GpuImageAddressMode mode);

		VkImage image = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;

		VkImageView swapchainView = VK_NULL_HANDLE;

	};

}
