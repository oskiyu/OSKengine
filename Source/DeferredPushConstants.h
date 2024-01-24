#pragma once

#include "Vector2.hpp"
#include "Vector4.hpp"

#include <glm/glm.hpp>

namespace OSK::GRAPHICS {
	
	struct DeferredPushConstants {
		glm::mat4 model;
		glm::mat4 previousModel;
		Vector2f resolution;
		float jitterIndex;
	};

}
