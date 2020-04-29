#pragma once

#include <glm.hpp>

namespace OSK::VULKAN {

	struct VulkanUBO {

		glm::mat4 model;

		glm::mat4 view;

		glm::mat4 projection;

	};

}