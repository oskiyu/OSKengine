#include "Quaternion.h"
#include "ToString.h"

using namespace OSK;

Quaternion::Quaternion() {
	Quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::normalize(Quat);
}

void Quaternion::Rotate_WorldSpace(float angle, const Vector3f& axis) {
	glm::quat rot = glm::normalize(glm::angleAxis(angle, axis.ToGLM()));

	Quat = Quat * (glm::inverse(Quat) * rot) * Quat;
}

void Quaternion::Rotate_LocalSpace(float angle, const Vector3f& axis) {
	Quat = Quat * glm::normalize(glm::angleAxis(angle, axis.ToGLM()));
}

Quaternion Quaternion::CreateFromEulerAngles(const Vector3f& rot) {
	Quaternion q;
	q.Quat = glm::quat(rot.ToGLM());

	return q;
}