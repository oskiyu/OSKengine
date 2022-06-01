#pragma once

#include "ISyncDevice.h"

#include "DynamicArray.hpp"

struct VkFence_T;
typedef VkFence_T* VkFence;
struct VkSemaphore_T;
typedef VkSemaphore_T* VkSemaphore;

namespace OSK::GRAPHICS {

	class GpuVulkan;
	class SwapchainVulkan;
	class CommandQueueVulkan;
	class CommandListVulkan;

	class OSKAPI_CALL SyncDeviceVulkan : public ISyncDevice {

	public:

		~SyncDeviceVulkan();
		SyncDeviceVulkan();

		void SetImageAvailableSemaphores(const DynamicArray<VkSemaphore>& semaphores);
		void SetRenderFinishedSemaphores(const DynamicArray<VkSemaphore>& semaphores);

		void SetInFlightFences(const DynamicArray<VkFence>& fences);
		void SetImagesInFlightFences(const DynamicArray<VkFence>& fences);

		void SetDevice(const GpuVulkan& device);
		void SetSwapchain(const SwapchainVulkan& swapchain);

		void FirstAwait();
		bool UpdateCurrentFrameIndex();
		void Flush(const CommandQueueVulkan& graphicsQueue, const CommandQueueVulkan& presentQueue, const CommandListVulkan& commandList);

		TSize GetCurrentFrameIndex() const;
		TSize GetCurrentCommandListIndex() const;

	private:

		DynamicArray<VkSemaphore> imageAvailableSemaphores;
		DynamicArray<VkSemaphore> renderFinishedSemaphores;

		DynamicArray<VkFence> inFlightFences;
		DynamicArray<VkFence> imagesInFlight;

		TSize framebufferIndex = 0;
		TSize currentFrame = 0;

		const GpuVulkan* device = nullptr;
		SwapchainVulkan* swapchain = nullptr;

	};

}
