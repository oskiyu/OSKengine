#include "VulkanSurface.h"

#include "VulkanInstance.h"

namespace OSK::VULKAN {

	void VulkanSurface::CreateSurface(GLFWwindow* ventana, const VulkanInstance& instance) {
		VkResult result = glfwCreateWindowSurface(instance.Instance, ventana, NULL, &Surface);
		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "ERROR: crear supericie : " + std::to_string(result), __LINE__);
		}
	}


	void VulkanSurface::Destroy(const VulkanInstance& instance) {
		vkDestroySurfaceKHR(instance.Instance, Surface, nullptr);
	}

}