#include "SyncDeviceVulkan.h"

#include <vulkan/vulkan.h>
#include "GpuVk.h"
#include "SwapchainVk.h"
#include "CommandQueueVk.h"
#include "CommandListVk.h"
#include "Assert.h"
#include "OSKengine.h"
#include "RendererVk.h"
#include "GpuVk.h"

#include "Logger.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

SyncDeviceVulkan::SyncDeviceVulkan() {
	
}

SyncDeviceVulkan::~SyncDeviceVulkan() {
	for (TSize i = 0; i < imageAvailableSemaphores.GetSize(); i++) {
		if (imageAvailableSemaphores[i] != VK_NULL_HANDLE) {
			vkDestroySemaphore(Engine::GetRenderer()->As<RendererVk>()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
				imageAvailableSemaphores[i], nullptr);
			imageAvailableSemaphores[i] = VK_NULL_HANDLE;
		}
	}

	for (TSize i = 0; i < renderFinishedSemaphores.GetSize(); i++) {
		if (renderFinishedSemaphores[i] != VK_NULL_HANDLE) {
			vkDestroySemaphore(Engine::GetRenderer()->As<RendererVk>()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
				renderFinishedSemaphores[i], nullptr);
			renderFinishedSemaphores[i] = VK_NULL_HANDLE;
		}
	}

	for (TSize i = 0; i < inFlightFences.GetSize(); i++) {
		if (inFlightFences[i] != VK_NULL_HANDLE) {
			vkDestroyFence(Engine::GetRenderer()->As<RendererVk>()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
				inFlightFences[i], nullptr);
			inFlightFences[i] = VK_NULL_HANDLE;
		}
	}
}

void SyncDeviceVulkan::SetImageAvailableSemaphores(const DynamicArray<VkSemaphore>& semaphores) {
	imageAvailableSemaphores = semaphores;
}

void SyncDeviceVulkan::SetRenderFinishedSemaphores(const DynamicArray<VkSemaphore>& semaphores) {
	renderFinishedSemaphores = semaphores;
}

void SyncDeviceVulkan::SetInFlightFences(const DynamicArray<VkFence>& fences) {
	inFlightFences = fences;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//vkCreateFence(device->GetLogicalDevice(), &fenceInfo, nullptr, &imageAcquireFence);
}

void SyncDeviceVulkan::SetImagesInFlightFences(const DynamicArray<VkFence>& fences) {
	imagesInFlight = { 0, 0, 0 };
}

void SyncDeviceVulkan::SetDevice(const GpuVk& device) {
	this->device = &device;
}

void SyncDeviceVulkan::SetSwapchain(const SwapchainVk& swapchain) {
	this->swapchain = const_cast<SwapchainVk*>(&swapchain);
}

void SyncDeviceVulkan::FirstAwait() {
	//vkWaitForFences(device->GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	//vkResetFences(device->GetLogicalDevice(), 1, &inFlightFences[currentFrame]);
}

bool SyncDeviceVulkan::UpdateCurrentFrameIndex() {
	// Adquirimos el índice de la próxima imagen a procesar.
	// NOTA: puede que tengamos que esperar a que esta imagen quede disponible.
	VkResult result = vkAcquireNextImageKHR(device->GetLogicalDevice(), swapchain->GetSwapchain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &framebufferIndex);
	OSK_CHECK(result == VK_SUCCESS, "vkAcquireNextImageKHR code: " + std::to_string(result));

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		//swapchain->Resize();

		return true;
	}

	return false;
}

void SyncDeviceVulkan::Flush(const CommandQueueVk& graphicsQueue, const CommandQueueVk& presentQueue, const CommandListVk& commandList) {

	//Esperar a que la imagen esté disponible antes de renderizar.
	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	// Submit Graphics
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame]; // Debemos esperar hasta que esta imagen esté disponible.
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame]; // Al terminar, indicamos que esta imagen se ha terminado de renderizar.
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandList.GetCommandBuffers()[currentFrame];

	VkResult result = vkQueueSubmit(graphicsQueue.GetQueue(), 1, &submitInfo, inFlightFences[currentFrame]);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido enviar la cola gráfica.");

	// Submit Compute
	VkSubmitInfo computeSubmitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame]; // Debemos esperar hasta que esta imagen esté disponible.
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame]; // Al terminar, indicamos que esta imagen se ha terminado de renderizar.
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandList.GetCommandBuffers()[currentFrame];

	// ---------------- PRESENT --------------------- //
	// Presentar la imagen renderizada en la pantalla.
	
	VkSwapchainKHR swapChains[] = { swapchain->GetSwapchain()};

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame]; // Debemos esperar a que la imagen actual termine de renderizarse.
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &framebufferIndex; // Lo presentamos en esta imagen.
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(presentQueue.GetQueue(), &presentInfo);
	const bool resized = result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR;
	if (resized) {
		// Esperamos a que se terminen todas las listas de comandos para
		// poder cambiar de tamaño los render targets.
		vkDeviceWaitIdle(device->GetLogicalDevice());
		/// @todo vkWaitForSemaphores

		swapchain->Resize();
		Engine::GetRenderer()->As<RendererVk>()->HandleResize();
	}

	currentFrame = (currentFrame + 1) % swapchain->GetImageCount();
	
	// Si la siguiente imagen está siendo procesada, esperar a que termine.
	if (!resized) {
		/// @todo vkGetSemaphoreState
		vkWaitForFences(device->GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(device->GetLogicalDevice(), 1, &inFlightFences[currentFrame]);
	}
}

TSize SyncDeviceVulkan::GetCurrentFrameIndex() const {
	return framebufferIndex;
}

TSize SyncDeviceVulkan::GetCurrentCommandListIndex() const {
	return currentFrame;
}
