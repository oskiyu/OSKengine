#include "BroadGjkDetector.h"

#include "Gjk.h"
#include "Simplex.h"
#include "EpaMinkowskiHull.h"
#include "BroadColliderHolder.h"

std::optional<bool> OSK::COLLISION::BroadGjkCollisionDetector::GetCollision(
	const BroadColliderHolder& first,
	const BroadColliderHolder& second) const
{
	const Simplex simplex = Simplex::GenerateFrom(*first.GetCollider(), *second.GetCollider());

	return simplex.ContainsOrigin();
}
