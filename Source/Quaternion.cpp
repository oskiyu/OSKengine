#include "Quaternion.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

using namespace OSK;


Quaternion Quaternion::Empty() {
	return Quaternion{};
}

Quaternion Quaternion::FromGlm(const glm::quat& quat) {
	Quaternion output;
	output.m_quaternion = quat;

	return output;
}


void Quaternion::Rotate_WorldSpace(float angle, const Vector3f& axis) {
	glm::quat rot = glm::normalize(glm::angleAxis(angle, axis.ToGlm()));

	m_quaternion = glm::normalize(m_quaternion * (glm::inverse(m_quaternion) * rot) * m_quaternion);
}

void Quaternion::Rotate_LocalSpace(float angle, const Vector3f& axis) {
	m_quaternion = glm::normalize(m_quaternion * glm::normalize(glm::angleAxis(angle, axis.ToGlm())));
}

Vector3f Quaternion::RotateVector(const Vector3f& vec) const {
	return Vector3f(glm::rotate(m_quaternion, vec.ToGlm()));
}


Quaternion Quaternion::operator+(const Quaternion& other) const {
	return Quaternion::FromGlm(glm::normalize(m_quaternion * other.m_quaternion));
}

Quaternion& Quaternion::operator+=(const Quaternion& other) {
	m_quaternion = glm::normalize(m_quaternion * other.m_quaternion);
	return *this;
}

Quaternion Quaternion::operator-(const Quaternion& other) const {
	return Quaternion::FromGlm(glm::normalize(glm::inverse(m_quaternion) * other.m_quaternion));
}

Quaternion& Quaternion::operator-=(const Quaternion& other) {
	m_quaternion = glm::normalize(glm::inverse(m_quaternion) * other.m_quaternion);
	return *this;
}


Vector3f Quaternion::ToEulerAngles() const {
	return Vector3f(glm::eulerAngles(m_quaternion));
}

glm::mat4 Quaternion::ToMat4() const {
	return glm::toMat4(m_quaternion);
}

glm::quat Quaternion::ToGlm() const {
	return m_quaternion;
}
