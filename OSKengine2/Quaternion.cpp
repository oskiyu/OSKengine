#include "Quaternion.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

using namespace OSK;

Quaternion::Quaternion() {

}

void Quaternion::Rotate_WorldSpace(float angle, const Vector3f& axis) {
	glm::quat rot = glm::normalize(glm::angleAxis(angle, axis.ToGlm()));

	quaternion = glm::normalize(quaternion * (glm::inverse(quaternion) * rot) * quaternion);
}

void Quaternion::Rotate_LocalSpace(float angle, const Vector3f& axis) {
	quaternion = glm::normalize(quaternion * glm::normalize(glm::angleAxis(angle, axis.ToGlm())));
}

Vector3f Quaternion::RotateVector(const Vector3f& vec) const {
	return Vector3f(glm::rotate(quaternion, vec.ToGlm()));
}

Vector3f Quaternion::ToEulerAngles() const {
	return Vector3f(glm::eulerAngles(quaternion));
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
