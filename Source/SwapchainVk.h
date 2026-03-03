#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "ISwapchain.h"
#include "VulkanTypedefs.h"

#include "VulkanTarget.h"

enum VkColorSpaceKHR;
OSK_VULKAN_TYPEDEF(VkSwapchainKHR);

namespace OSK::GRAPHICS {

	enum class Format;

	template <VulkanTarget> class GpuVk;
	template <VulkanTarget> class GpuImageVk;

	class RenderpassVulkan;

	template <VulkanTarget Target>
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
			const GpuVk<Target>& device,
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
			const GpuVk<Target>& device,
			const Vector2ui& resolution,
			std::span<const UIndex32> queueIndices);

		/// @throws SwapchainCreationException Si ocurre algún error.
		void AcquireImages(const GpuVk<Target>& device);
		/// @throws SwapchainCreationException Si ocurre algún error.
		void AcquireViews(const GpuVk<Target>& device);

		/// <summary>
		/// Devuelve el mejor formato de colores soportado por el monitor.
		/// </summary>
		static VkColorSpaceKHR GetSupportedColorSpace(const GpuVk<Target>& device);

		VkSwapchainKHR m_swapchain = nullptr;
		DynamicArray<UIndex32> m_queueIndices;

	};

	template class SwapchainVk<VulkanTarget::VK_1_0>;
	template class SwapchainVk<VulkanTarget::VK_LATEST>;

}

#endif
