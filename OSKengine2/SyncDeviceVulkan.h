#pragma once

#include "ISyncDevice.h"

#include <vector>

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

		void SetImageAvailableSemaphores(const std::vector<VkSemaphore>& semaphores);
		void SetRenderFinishedSemaphores(const std::vector<VkSemaphore>& semaphores);

		void SetInFlightFences(const std::vector<VkFence>& fences);
		void SetImagesInFlightFences(const std::vector<VkFence>& fences);

		void SetFences(const std::vector<VkFence>& fences);

		void SetDevice(const GpuVulkan& device);
		void SetSwapchain(const SwapchainVulkan& swapchain);

		void FirstAwait();
		bool UpdateCurrentFrameIndex();
		void Flush(const CommandQueueVulkan& graphicsQueue, const CommandQueueVulkan& presentQueue, const CommandListVulkan& commandList);

	private:

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;

		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;

		std::vector<VkFence> fences;

		unsigned int currentFrame = 0;
		unsigned int nextFrame = 0;

		const GpuVulkan* device = nullptr;
		SwapchainVulkan* swapchain = nullptr;

	};

}
