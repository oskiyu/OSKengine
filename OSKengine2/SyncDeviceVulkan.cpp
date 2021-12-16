#include "SyncDeviceVulkan.h"

#include <vulkan/vulkan.h>
#include "GpuVulkan.h"
#include "SwapchainVulkan.h"
#include "CommandQueueVulkan.h"
#include "CommandListVulkan.h"
#include "Assert.h"
#include "OSKengine.h"
#include "RendererVulkan.h"
#include "GpuVulkan.h"

using namespace OSK;

SyncDeviceVulkan::~SyncDeviceVulkan() {
	for (auto i : imageAvailableSemaphores)
		vkDestroySemaphore(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
			i, nullptr);
	for (auto i : renderFinishedSemaphores)
		vkDestroySemaphore(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
			i, nullptr);

	for (auto i : inFlightFences)
		vkDestroyFence(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
			i, nullptr);
	for (auto i : imagesInFlight)
		vkDestroyFence(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
			i, nullptr);
}

void SyncDeviceVulkan::SetImageAvailableSemaphores(const DynamicArray<VkSemaphore>& semaphores) {
	imageAvailableSemaphores = semaphores;
}

void SyncDeviceVulkan::SetRenderFinishedSemaphores(const DynamicArray<VkSemaphore>& semaphores) {
	renderFinishedSemaphores = semaphores;
}

void SyncDeviceVulkan::SetInFlightFences(const DynamicArray<VkFence>& fences) {
	inFlightFences = fences;
}

void SyncDeviceVulkan::SetImagesInFlightFences(const DynamicArray<VkFence>& fences) {
	imagesInFlight = fences;
}

void SyncDeviceVulkan::SetDevice(const GpuVulkan& device) {
	this->device = &device;
}

void SyncDeviceVulkan::SetSwapchain(const SwapchainVulkan& swapchain) {
	this->swapchain = const_cast<SwapchainVulkan*>(&swapchain);
}

void SyncDeviceVulkan::FirstAwait() {
	vkWaitForFences(device->GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
}

bool SyncDeviceVulkan::UpdateCurrentFrameIndex() {
	VkResult result = vkAcquireNextImageKHR(device->GetLogicalDevice(), swapchain->GetSwapchain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &nextFrame);
	
	if (imagesInFlight[nextFrame] != VK_NULL_HANDLE)
		vkWaitForFences(device->GetLogicalDevice(), 1, &imagesInFlight[nextFrame], VK_TRUE, UINT64_MAX);
	imagesInFlight[nextFrame] = inFlightFences[currentFrame];

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		swapchain->Resize();

		return true;
	}

	return false;
}

void SyncDeviceVulkan::Flush(const CommandQueueVulkan& graphicsQueue, const CommandQueueVulkan& presentQueue, const CommandListVulkan& commandList) {
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	//Esperar a que la imagen esté disponible antes de renderizar.
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = &imageAvailableSemaphores[0];
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandList.GetCommandBuffers()[nextFrame];

	//Semáforos a los que vamos a avisar cuando se termine de renderizar la imagen.
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device->GetLogicalDevice(), 1, &inFlightFences[currentFrame]);

	VkResult result = vkQueueSubmit(graphicsQueue.GetQueue(), 1, &submitInfo, inFlightFences[currentFrame]);
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

	result = vkQueuePresentKHR(presentQueue.GetQueue(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		swapchain->Resize();

	vkQueueWaitIdle(presentQueue.GetQueue());

	currentFrame = (currentFrame + 1) % swapchain->GetImageCount();
}
