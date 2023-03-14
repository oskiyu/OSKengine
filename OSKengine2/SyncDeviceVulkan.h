#pragma once

#include "ISyncDevice.h"

#include "DynamicArray.hpp"

struct VkFence_T;
typedef VkFence_T* VkFence;
struct VkSemaphore_T;
typedef VkSemaphore_T* VkSemaphore;

namespace OSK::GRAPHICS {

	class GpuVk;
	class SwapchainVk;
	class CommandQueueVk;
	class CommandListVk;

	class OSKAPI_CALL SyncDeviceVulkan : public ISyncDevice {

	public:

		~SyncDeviceVulkan();
		SyncDeviceVulkan();

		void SetImageAvailableSemaphores(const DynamicArray<VkSemaphore>& semaphores);
		void SetRenderFinishedSemaphores(const DynamicArray<VkSemaphore>& semaphores);

		void SetInFlightFences(const DynamicArray<VkFence>& fences);
		void SetImagesInFlightFences(const DynamicArray<VkFence>& fences);

		void SetDevice(const GpuVk& device);
		void SetSwapchain(const SwapchainVk& swapchain);

		void FirstAwait();
		bool UpdateCurrentFrameIndex();
		void Flush(const CommandQueueVk& graphicsQueue, const CommandQueueVk& presentQueue, const CommandListVk& commandList);

		TSize GetCurrentFrameIndex() const;
		TSize GetCurrentCommandListIndex() const;

	private:

		DynamicArray<VkSemaphore> imageAvailableSemaphores;
		DynamicArray<VkSemaphore> renderFinishedSemaphores;

		DynamicArray<VkFence> inFlightFences;
		DynamicArray<VkFence> imagesInFlight;

		TSize framebufferIndex = 0;
		TSize currentFrame = 0;

		const GpuVk* device = nullptr;
		SwapchainVk* swapchain = nullptr;

	};

}
