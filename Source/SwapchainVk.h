#pragma once

#include "ISwapchain.h"
#include "VulkanTypedefs.h"

enum VkColorSpaceKHR;
OSK_VULKAN_TYPEDEF(VkSwapchainKHR);

namespace OSK::GRAPHICS {

	enum class Format;
	class GpuVk;
	class GpuImageVk;
	class RenderpassVulkan;

	class OSKAPI_CALL SwapchainVk final : public ISwapchain {

	public:

		/// @brief 
		/// @param mode 
		/// @param format 
		/// @param device 
		/// @param display 
		/// @param queueIndices 
		/// 
		/// @throws SwapchainCreationException Si ocurre algún error.
		SwapchainVk(
			PresentMode mode,
			Format format,
			const GpuVk& device,
			const Vector2ui& resolution,
			std::span<const UIndex32> queueIndices);
		~SwapchainVk() override;


		void Resize(
			const IGpu& gpu,
			Vector2ui newResolution) override;

		void Present() override;

		VkSwapchainKHR GetSwapchain() const;

	private:

		void CreationLogic(
			const GpuVk& device,
			const Vector2ui& resolution);

		/// @throws SwapchainCreationException Si ocurre algún error.
		void AcquireImages(
			const GpuVk& device, 
			const Vector2ui& resolution);
		/// @throws SwapchainCreationException Si ocurre algún error.
		void AcquireViews(const GpuVk& device);

		/// <summary>
		/// Devuelve el mejor formato de colores soportado por el monitor.
		/// </summary>
		static VkColorSpaceKHR GetSupportedColorSpace(const GpuVk& device);

		VkSwapchainKHR m_swapchain = nullptr;

	};

}
