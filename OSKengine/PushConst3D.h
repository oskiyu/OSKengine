#pragma once

#include <glm.hpp>

namespace OSK {

	struct PushConst3D {
		alignas(16) glm::mat4 model;
	};

}
