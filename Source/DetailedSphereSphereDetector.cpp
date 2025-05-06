#include "DetailedSphereSphereDetector.h"

#include "SphereCollider.h"
#include "NarrowColliderHolder.h"

std::optional<OSK::COLLISION::DetailedCollisionInfo> OSK::COLLISION::DetailedSphereSphereDetector::GetCollision(
	const NarrowColliderHolder& first,
	const NarrowColliderHolder& second) const
{
	if (!(first.GetCollider()->Is<SphereCollider>() && second.GetCollider()->Is<SphereCollider>())) {
		return {};
	}

	const auto& castedFirst  = *first.GetCollider()->As<SphereCollider>();
	const auto& castedSecond = *second.GetCollider()->As<SphereCollider>();

	const auto& firstPosition  = first.GetTransform().GetPosition();
	const auto& secondPosition = second.GetTransform().GetPosition();

	const auto distance2 = firstPosition.GetDistanceTo2(secondPosition);
	const auto radii = castedFirst.GetRadius() + castedSecond.GetRadius();

	if (distance2 > radii * radii) {
		return DetailedCollisionInfo::False();
	}

	const auto normal = (secondPosition - firstPosition).GetNormalized();
	const auto mtv = normal * (firstPosition.GetDistanceTo(secondPosition) - radii);
	const auto point = firstPosition + mtv;

	return DetailedCollisionInfo::True(
		mtv,
		{ point },
		normal,
		-normal,
		DetailedCollisionInfo::MtvDirection::A_TO_B);
}
