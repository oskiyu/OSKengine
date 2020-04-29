#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#include <vulkan/vulkan.h>

#include "Log.h"

namespace OSK::VULKAN {

	struct VulkanInstance;

	struct VulkanSurface {

	public:

		VkSurfaceKHR Surface;


		VkQueue SurfaceQueue;


		void CreateSurface(GLFWwindow* ventana, const VulkanInstance& instance);


		void Destroy(const VulkanInstance& instance);

	};

}