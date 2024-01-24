#include "Vector3.hpp"

#include <glm/glm.hpp>

namespace OSK::Math {

	inline static Vector3f TransformPoint(const Vector3f& point, const glm::mat4& matrix) {
		const glm::vec4 p = glm::vec4(point.ToGlm(), 1.0f);
		const glm::vec4 transformedPoint = matrix * p;

		return Vector3f(glm::vec3(transformedPoint) / transformedPoint.w);
	}

	inline static Vector3f TranslatePoint(const Vector3f& point, const glm::mat4& matrix) {
		const glm::vec4 p = glm::vec4(point.ToGlm(), 1.0f);
		const glm::vec4 transformedPoint = matrix * p;

		return Vector3f(glm::vec3(transformedPoint) / transformedPoint.w);
	}

	inline static Vector3f PositionFromMatrix(const glm::mat4 matrix) {
		return Vector3f(glm::vec3(matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	}

}
