#include "RayCastResult.h"

using namespace OSK;
using namespace OSK::COLLISION;

RayCastResult::RayCastResult(bool intersection, const Vector3f& point)
	: intersection(intersection), intersectionPoint(point) { 
	
}

RayCastResult RayCastResult::False() {
	return RayCastResult(false, 0.0f);
}

RayCastResult RayCastResult::True(const Vector3f& intersection) {
	return RayCastResult(true, intersection);
}

bool RayCastResult::Result() const {
	return intersection;
}

const Vector3f& RayCastResult::GetIntersectionPoint() const {
	return intersectionPoint;
}


