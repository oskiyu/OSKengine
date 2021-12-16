#pragma once

#include "ISyncDevice.h"

#include "DynamicArray.hpp"

struct VkFence_T;
typedef VkFence_T* VkFence;
struct VkSemaphore_T;
typedef VkSemaphore_T* VkSemaphore;

namespace OSK {

	class GpuVulkan;
	class SwapchainVulkan;
	class CommandQueueVulkan;
	class CommandListVulkan;

	class OSKAPI_CALL SyncDeviceVulkan : public ISyncDevice {

	public:

		~SyncDeviceVulkan();

		void SetImageAvailableSemaphores(const DynamicArray<VkSemaphore>& semaphores);
		void SetRenderFinishedSemaphores(const DynamicArray<VkSemaphore>& semaphores);

		void SetInFlightFences(const DynamicArray<VkFence>& fences);
		void SetImagesInFlightFences(const DynamicArray<VkFence>& fences);

		void SetDevice(const GpuVulkan& device);
		void SetSwapchain(const SwapchainVulkan& swapchain);

		void FirstAwait();
		bool UpdateCurrentFrameIndex();
		void Flush(const CommandQueueVulkan& graphicsQueue, const CommandQueueVulkan& presentQueue, const CommandListVulkan& commandList);

	private:

		DynamicArray<VkSemaphore> imageAvailableSemaphores;
		DynamicArray<VkSemaphore> renderFinishedSemaphores;

		DynamicArray<VkFence> inFlightFences;
		DynamicArray<VkFence> imagesInFlight;

		unsigned int currentFrame = 0;
		unsigned int nextFrame = 0;

		const GpuVulkan* device = nullptr;
		SwapchainVulkan* swapchain = nullptr;

	};

}
