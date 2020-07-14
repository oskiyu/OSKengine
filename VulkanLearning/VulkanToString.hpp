#pragma once

#include <string>
#include <vulkan/vulkan.h>

inline std::string ToString(VkPresentModeKHR presentMode) {
	switch (presentMode) {
		case VK_PRESENT_MODE_FIFO_KHR:
			return std::string("VK_PRESENT_MODE_FIFO_KHR");
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
			return std::string("VK_PRESENT_MODE_FIFO_RELAXED_KHR");
		case VK_PRESENT_MODE_IMMEDIATE_KHR:
			return std::string("VK_PRESENT_MODE_IMMEDIATE_KHR");
		case VK_PRESENT_MODE_MAILBOX_KHR:
			return std::string("VK_PRESENT_MODE_MAILBOX_KHR");
		
		default:
			return std::string("");
	}
}
