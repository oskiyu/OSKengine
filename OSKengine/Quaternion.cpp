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

Quaternion Quaternion::Interpolate(const Quaternion& other, float factor) const {   // calc cosine theta
    float cosom = Quat.x * other.Quat.x + Quat.y * other.Quat.y + Quat.z * other.Quat.z + Quat.w * other.Quat.w;

    // adjust signs (if necessary)
    Quaternion end = other;
    if (cosom < static_cast<float>(0.0))
    {
        cosom = -cosom;
        end.Quat.x = -end.Quat.x;   // Reverse all signs
        end.Quat.y = -end.Quat.y;
        end.Quat.z = -end.Quat.z;
        end.Quat.w = -end.Quat.w;
    }

    // Calculate coefficients
    float sclp, sclq;
    if ((static_cast<float>(1.0) - cosom) > static_cast<float>(0.0001)) // 0.0001 -> some epsillon
    {
        // Standard case (slerp)
        float omega, sinom;
        omega = std::acos(cosom); // extract theta from dot product's cos theta
        sinom = std::sin(omega);
        sclp = std::sin((static_cast<float>(1.0) - factor) * omega) / sinom;
        sclq = std::sin(factor * omega) / sinom;
    }
    else
    {
        // Very close, do linear interp (because it's faster)
        sclp = static_cast<float>(1.0) - factor;
        sclq = factor;
    }

    Quaternion output;
    output.Quat.x = sclp * Quat.x + sclq * end.Quat.x;
    output.Quat.y = sclp * Quat.y + sclq * end.Quat.y;
    output.Quat.z = sclp * Quat.z + sclq * end.Quat.z;
    output.Quat.w = sclp * Quat.w + sclq * end.Quat.w;

    return output;
}