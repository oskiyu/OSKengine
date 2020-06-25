#pragma once

#include <glm.hpp>

struct UniformBufferObject {
	alignas(16) glm::mat4 Model;
	alignas(16) glm::mat4 View;
	alignas(16) glm::mat4 Projection;
};
