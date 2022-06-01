#pragma once

#include "IGpuImage.h"

struct VkImage_T;
typedef VkImage_T* VkImage;

struct VkImageView_T;
typedef VkImageView_T* VkImageView;

struct VkSampler_T;
typedef VkSampler_T* VkSampler;

namespace OSK::GRAPHICS {
	
	class OSKAPI_CALL GpuImageVulkan : public GpuImage {

	public:

		GpuImageVulkan(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format);
		~GpuImageVulkan();

		void SetImage(VkImage image);
		VkImage GetImage() const;

		void SetView(VkImageView view);
		VkImageView GetView() const;

		void SetSampler(VkSampler sampler);
		VkSampler GetSampler() const;

	private:

		VkImage image = 0;
		VkImageView view = 0;
		VkSampler sampler = 0;

	};

}
