#pragma once

#include <glm.hpp>

namespace OSK {

	struct OSKAPI_CALL PushConst3D {
		alignas(16) glm::mat4 model;
	};

}
