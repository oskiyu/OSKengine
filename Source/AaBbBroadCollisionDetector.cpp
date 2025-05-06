#include "AabbAabbDetector.h"

#include "AxisAlignedBoundingBox.h"
#include "BroadColliderHolder.h"

std::optional<bool> OSK::COLLISION::AabbAabbCollisionDetector::GetCollision(
	const BroadColliderHolder& first,
	const BroadColliderHolder& second) const
{
	if (!(first.GetCollider()->Is<AxisAlignedBoundingBox>() && second.GetCollider()->Is<AxisAlignedBoundingBox>())) {
		return {};
	}

	const Vector3f firstMax  = first.GetCollider()->As<AxisAlignedBoundingBox>()->GetMax();
	const Vector3f secondMax = second.GetCollider()->As<AxisAlignedBoundingBox>()->GetMax();

	const Vector3f firstPosition  = first.GetPosition();
	const Vector3f secondPosition = second.GetPosition();

	return firstPosition.x <= secondMax.x && firstMax.x >= secondPosition.x
		&& firstPosition.y <= secondMax.y && firstMax.y >= secondPosition.y
		&& firstPosition.z <= secondMax.z && firstMax.z >= secondPosition.z;
}
