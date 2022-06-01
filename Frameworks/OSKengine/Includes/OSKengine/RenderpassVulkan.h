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

namespace OSK::GRAPHICS {

	class ISwapchain;
	class GpuImage;
	enum class Format;

	class OSKAPI_CALL RenderpassVulkan : public IRenderpass {

	public:

		RenderpassVulkan(RenderpassType type);
		~RenderpassVulkan();

		void Create(const ISwapchain* swapchain, Format format);
		void CreateFinalPresent(const ISwapchain* swapchain);

		void SetImages(GpuImage* image0, GpuImage* image1, GpuImage* image2) override;

		VkRenderPass GetRenderpass() const;
		VkFramebuffer GetFramebuffer(TSize index) const;

	private:

		void Create(const ISwapchain* swapchain, VkImageLayout finalLayout, Format format);

		VkRenderPass renderpass = 0;
		DynamicArray<VkFramebuffer> framebuffers;

	};

}
