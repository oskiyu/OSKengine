#pragma once

#include <glm.hpp>
#include <vector>

namespace OSK{

	struct OSKAPI_CALL UBO {
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;
		alignas(16) glm::mat4 projection2D;

		alignas(16) glm::mat4 bones[64]{glm::mat4(1.0f)};

		alignas(16) glm::vec3 cameraPos;
	};

}
