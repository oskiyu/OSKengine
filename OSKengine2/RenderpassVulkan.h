#pragma once

#include "OSKmacros.h"
#include "IRenderpass.h"
#include "DynamicArray.hpp"
#include "UniquePtr.hpp"

struct VkRenderPass_T;
typedef VkRenderPass_T* VkRenderPass;

struct VkFramebuffer_T;
typedef VkFramebuffer_T* VkFramebuffer;

enum VkImageLayout;

namespace OSK {

	class ISwapchain;
	class GpuImage;

	class OSKAPI_CALL RenderpassVulkan : public IRenderpass {

	public:

		RenderpassVulkan(RenderpassType type);
		~RenderpassVulkan();

		void Create(const ISwapchain* swapchain);
		void CreateFinalPresent(const ISwapchain* swapchain);

		void SetImage(GpuImage* image) override;
		void SetImages(GpuImage* image0, GpuImage* image1, GpuImage* image2) override;

		VkRenderPass GetRenderpass() const;
		VkFramebuffer GetFramebuffer(TSize index) const;

	private:

		void Create(const ISwapchain* swapchain, VkImageLayout finalLayout);

		VkRenderPass renderpass = 0;
		DynamicArray<VkFramebuffer> framebuffers;

		UniquePtr<GpuImage> colorImgs[3];
		UniquePtr<GpuImage> depthImgs[3];

	};

}
