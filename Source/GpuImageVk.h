#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IGpuImage.h"
#include "VulkanTypedefs.h"
#include "DynamicArray.hpp"
#include "NumericTypes.h"

OSK_VULKAN_TYPEDEF(VkImage);
OSK_VULKAN_TYPEDEF(VkSampler);
OSK_VULKAN_TYPEDEF(VkImageView);
OSK_VULKAN_FLAGS_TYPEDEF(VkImageAspectFlags);

enum VkImageViewType;
enum VkImageType;
enum VkFilter;
enum VkSamplerAddressMode;

namespace OSK::GRAPHICS {
	
	class OSKAPI_CALL GpuImageVk final : public GpuImage {

	public:

		GpuImageVk(
			const GpuImageCreateInfo& info,
			const ICommandQueue* ownerQueue);

		~GpuImageVk() override;

		OSK_DISABLE_COPY(GpuImageVk);


		/// @brief Crea la imagen con los parámetros pasados por el constructor.
		/// @throws ImageCreationException si no se puede crear la imagen en la GPU.
		void CreateVkImage();
		VkImage GetVkImage() const;

		void _SetVkImage(VkImage img);
		
		void SetSwapchainView(VkImageView view);
		VkImageView GetSwapchainView() const;

		void SetDebugName(const std::string& name) override;

		static VkImageAspectFlags GetAspectFlags(SampledChannel channel);

	protected:

		OwnedPtr<IGpuImageView> CreateView(const GpuImageViewConfig& viewConfig) const override;

	private:

		VkImageType GetVkImageType() const;
		VkImageViewType GetVkImageViewType() const;
		VkImageViewType GetVkImageArrayViewType() const;

		VkImage m_image = nullptr;

		VkImageView m_swapchainView = nullptr;

		std::string m_name = "";

	};

}

#endif
