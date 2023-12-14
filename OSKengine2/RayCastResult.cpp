#include "RayCastResult.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

RayCastResult::RayCastResult(bool intersection, const Vector3f& point, GameObjectIndex object)
	: m_intersectionPoint(point), m_intersection(intersection), m_object(object) { 
	
}

RayCastResult RayCastResult::False() {
	return RayCastResult(false, Vector3f::Zero, EMPTY_GAME_OBJECT);
}

RayCastResult RayCastResult::True(const Vector3f& intersection, GameObjectIndex obj) {
	return RayCastResult(true, intersection, obj);
}

bool RayCastResult::Result() const {
	return m_intersection;
}

const Vector3f& RayCastResult::GetIntersectionPoint() const {
	return m_intersectionPoint;
}

ECS::GameObjectIndex RayCastResult::GetObject() const {
	return m_object;
}
