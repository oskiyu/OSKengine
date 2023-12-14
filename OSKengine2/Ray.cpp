#include "Ray.h"

using namespace OSK;
using namespace OSK::COLLISION;

RayCastResult Ray::IntersectionWithPlane(const Vector3f& planeNormal, const Vector3f& planePoint) const {
	// Ray eq.: p = ray.origin + ray.direction * distance
	// Plane eq.: (p - planePoint) dot planeNormal = 0
	//
	// https://gfxcourses.stanford.edu/cs348b/spring22content/media/intersection/rt1_3GyBK6F.pdf

	const float distance = (planePoint - origin).Dot(planeNormal) / direction.Dot(planeNormal);

	if (distance > std::numeric_limits<float>::max() || distance < std::numeric_limits<float>::min()) {
		return RayCastResult::False();
	}
	else {
		return RayCastResult::True(origin + direction * distance, 0);
	}
}
