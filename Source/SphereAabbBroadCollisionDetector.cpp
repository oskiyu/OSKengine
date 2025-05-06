#include "SphereAabbBroadCollisionDetector.h"

#include "AxisAlignedBoundingBox.h"
#include "SphereCollider.h"
#include "BroadColliderHolder.h"

std::optional<bool> OSK::COLLISION::SphereAabbCollisionDetector::GetCollision(
	const BroadColliderHolder& first,
	const BroadColliderHolder& second) const
{

	const AxisAlignedBoundingBox* box = nullptr;
	const SphereCollider* sphere = nullptr;

	const BroadColliderHolder* boxHolder = nullptr;
	const BroadColliderHolder* sphereHolder = nullptr;

	if (first.GetCollider()->Is<AxisAlignedBoundingBox>() && second.GetCollider()->Is<SphereCollider>()) {
		box = first.GetCollider()->As<AxisAlignedBoundingBox>();
		sphere = second.GetCollider()->As<SphereCollider>();

		boxHolder = &first;
		sphereHolder = &second;
	}
	else if (first.GetCollider()->Is<SphereCollider>() && second.GetCollider()->Is<AxisAlignedBoundingBox>()) {
		sphere = first.GetCollider()->As<SphereCollider>();
		box = second.GetCollider()->As<AxisAlignedBoundingBox>();

		boxHolder = &second;
		sphereHolder = &first;
	}
	else {
		return {};
	}

	// Punto de la caja más cercano a la esfera.
	const Vector3f point = {
		glm::max(box->GetMin().x, glm::min(sphereHolder->GetPosition().x, box->GetMax().x)),
		glm::max(box->GetMin().y, glm::min(sphereHolder->GetPosition().y, box->GetMax().y)),
		glm::max(box->GetMin().z, glm::min(sphereHolder->GetPosition().z, box->GetMax().z))
	};

	return sphere->IBroadCollider::ContainsPoint(point);
}
