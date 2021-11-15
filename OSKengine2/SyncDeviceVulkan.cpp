#include "SyncDeviceVulkan.h"

#include <vulkan/vulkan.h>
#include "GpuVulkan.h"
#include "SwapchainVulkan.h"
#include "CommandQueueVulkan.h"
#include "CommandListVulkan.h"
#include "Assert.h"

using namespace OSK;

void SyncDeviceVulkan::SetImageAvailableSemaphores(const std::vector<VkSemaphore>& semaphores) {
	imageAvailableSemaphores = semaphores;
}

void SyncDeviceVulkan::SetRenderFinishedSemaphores(const std::vector<VkSemaphore>& semaphores) {
	renderFinishedSemaphores = semaphores;
}

void SyncDeviceVulkan::SetInFlightFences(const std::vector<VkFence>& fences) {
	inFlightFences = fences;
}

void SyncDeviceVulkan::SetImagesInFlightFences(const std::vector<VkFence>& fences) {
	imagesInFlight = fences;
}

void SyncDeviceVulkan::SetFences(const std::vector<VkFence>& fences) {
	this->fences = fences;
}

void SyncDeviceVulkan::SetDevice(const GpuVulkan& device) {
	this->device = &device;
}

void SyncDeviceVulkan::SetSwapchain(const SwapchainVulkan& swapchain) {
	this->swapchain = const_cast<SwapchainVulkan*>(&swapchain);
}

void SyncDeviceVulkan::FirstAwait() {
	vkWaitForFences(device->GetLogicalDevice(), 1, &fences[currentFrame], VK_TRUE, UINT64_MAX);
}

bool SyncDeviceVulkan::UpdateCurrentFrameIndex() {
	VkResult result = vkAcquireNextImageKHR(device->GetLogicalDevice(), swapchain->GetSwapchain(), UINT64_MAX, imageAvailableSemaphores[0], VK_NULL_HANDLE, &nextFrame);
	vkWaitForFences(device->GetLogicalDevice(), 1, &fences[nextFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(device->GetLogicalDevice(), 1, &fences[nextFrame]);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		swapchain->Resize();

		return true;
	}

	if (imagesInFlight[nextFrame] != VK_NULL_HANDLE)
		vkWaitForFences(device->GetLogicalDevice(), 1, &imagesInFlight[nextFrame], VK_TRUE, UINT64_MAX);

	imagesInFlight[nextFrame] = inFlightFences[currentFrame];
	vkResetFences(device->GetLogicalDevice(), 1, &fences[nextFrame]);

	return false;
}

void SyncDeviceVulkan::Flush(const CommandQueueVulkan& graphicsQueue, const CommandQueueVulkan& presentQueue, const CommandListVulkan& commandList) {
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	//Esperar a que la imagen esté disponible antes de renderizar.
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[0] };
	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = &imageAvailableSemaphores[0];
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandList.GetCommandBuffers()[nextFrame];

	//Semáforos a los que vamos a avisar cuando se termine de renderizar la imagen.
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[0] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VkResult result = vkQueueSubmit(graphicsQueue.GetQueue(), 1, &submitInfo, fences[nextFrame]);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido enviar la cola gráfica.");

	// ---------------- PRESENT --------------------- //
	//Presentar la imagen renderizada en la pantalla.
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { swapchain->GetSwapchain()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &nextFrame;
	presentInfo.pResults = nullptr;

	vkWaitForFences(device->GetLogicalDevice(), 1, &fences[nextFrame], VK_TRUE, UINT64_MAX);
	result = vkQueuePresentKHR(presentQueue.GetQueue(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		swapchain->Resize();

	vkQueueWaitIdle(presentQueue.GetQueue());

	currentFrame = (currentFrame + 1) % swapchain->GetImageCount();
}
