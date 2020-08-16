#pragma once

#include <glm.hpp>

namespace OSK{

	struct UBO {
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;
		alignas(16) glm::mat4 projection2D;

		 glm::mat4 bones[64]{glm::mat4(1.0f)};
	};

}
