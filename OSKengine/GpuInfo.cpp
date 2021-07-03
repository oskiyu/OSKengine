#include "GpuInfo.h"

#include "VulkanRenderer.h"

using namespace OSK;
using namespace OSK::VULKAN;

void GpuInfo::SetMaxMsaa() {
    VkSampleCountFlags count = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;

    if (count & VK_SAMPLE_COUNT_64_BIT)
        maxMsaaSamples = VK_SAMPLE_COUNT_64_BIT;

    else if (count & VK_SAMPLE_COUNT_32_BIT)
        maxMsaaSamples = VK_SAMPLE_COUNT_32_BIT;

    else if (count & VK_SAMPLE_COUNT_16_BIT)
        maxMsaaSamples = VK_SAMPLE_COUNT_16_BIT;

    else if (count & VK_SAMPLE_COUNT_8_BIT) 
        maxMsaaSamples = VK_SAMPLE_COUNT_8_BIT;

    else if (count & VK_SAMPLE_COUNT_4_BIT) 
        maxMsaaSamples = VK_SAMPLE_COUNT_4_BIT;

    else if (count & VK_SAMPLE_COUNT_2_BIT) 
        maxMsaaSamples = VK_SAMPLE_COUNT_2_BIT;

    else
        maxMsaaSamples = VK_SAMPLE_COUNT_1_BIT;

    maxMsaaSamples = VK_SAMPLE_COUNT_4_BIT;
}

void GpuInfo::UpdateSwapchainSupport() {
    //Obtener las capacidades.
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, renderer->surface, &shapchainSupport.surfaceCapabilities);

    //Número de formatos soportados.
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, renderer->surface, &formatCount, nullptr);

    //Obtener formatos soportados.
    shapchainSupport.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, renderer->surface, &formatCount, shapchainSupport.formats.data());

    //Números de modos de presentación.
    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, renderer->surface, &presentModeCount, nullptr);
    
    //Obtener modos de presentación.
    shapchainSupport.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, renderer->surface, &formatCount, shapchainSupport.presentModes.data());
}