#include "AnimUBO.h"

using namespace OSK;

AnimUBO::AnimUBO() {
	for (auto& i : bones) {
		i = glm::mat4(1.0f);
	}
}