#include "SphereSphereBroadCollisionDetector.h"

#include "BroadColliderHolder.h"
#include "SphereCollider.h"

std::optional<bool> OSK::COLLISION::SphereSphereBroadCollisionDetector::GetCollision(
	const BroadColliderHolder& first,
	const BroadColliderHolder& second) const
{
	if (!(first.GetCollider()->Is<SphereCollider>() && second.GetCollider()->Is<SphereCollider>())) {
		return {};
	}

	// Optimización: elevamos al cuadrado toda la inecuación para
	// así evitar un cálculo de raíz cuadrada, que es mas lento que
	// calcular un número al cuadrado.
	//
	// distance < radioA + radioB
	//
	// distance^2 < (radioA + radioB)^2
	const float distance2 = first.GetPosition().GetDistanceTo2(second.GetPosition());

	const float sumRadius = first.GetCollider()->As<SphereCollider>()->GetRadius() 
							+ second.GetCollider()->As<SphereCollider>()->GetRadius();
	const float sumRadius2 = sumRadius * sumRadius;

	return distance2 < sumRadius2;
}
