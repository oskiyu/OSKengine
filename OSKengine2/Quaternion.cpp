#include "Quaternion.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>

using namespace OSK;

Quaternion::Quaternion() {
	quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::normalize(quaternion);
}

void Quaternion::Rotate_WorldSpace(float angle, const Vector3f& axis) {
	glm::quat rot = glm::normalize(glm::angleAxis(angle, axis.ToGLM()));

	quaternion = quaternion * (glm::inverse(quaternion) * rot) * quaternion;
}

void Quaternion::Rotate_LocalSpace(float angle, const Vector3f& axis) {
	quaternion = quaternion * glm::normalize(glm::angleAxis(angle, axis.ToGLM()));
}

Vector3f Quaternion::ToEulerAngles() const {
	return glm::eulerAngles(quaternion);
}

Quaternion Quaternion::FromGlm(const glm::quat& quat) {
	Quaternion output;
	output.quaternion = quat;

	return output;
}

glm::mat4 Quaternion::ToMat4() const {
	return glm::toMat4(quaternion);
}

glm::quat Quaternion::ToGlm() const {
	return quaternion;
}
