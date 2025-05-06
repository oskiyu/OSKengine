#include "GjkEpaDetector.h"

#include "Gjk.h"
#include "Simplex.h"
#include "EpaMinkowskiHull.h"
#include "NarrowColliderHolder.h"

std::optional<OSK::COLLISION::DetailedCollisionInfo> OSK::COLLISION::GjkEpaDetector::GetCollision(
	const NarrowColliderHolder& first,
	const NarrowColliderHolder& second) const
{
	const Simplex simplex = Simplex::GenerateFrom(*first.GetCollider(), *second.GetCollider());

	if (!simplex.ContainsOrigin()) {
		return DetailedCollisionInfo::False();
	}

	const auto minkowskiHull = EpaMinkowskiHull::From(simplex, *first.GetCollider(), *second.GetCollider());

	if (!minkowskiHull.SurpasesMinDepth()) {
		return DetailedCollisionInfo::False();
	}

	const auto mtv = minkowskiHull.GetMtv();

	const Vector3f normal = mtv.GetNormalized();
	const Vector3f otherNormal = -normal;

	return DetailedCollisionInfo::True(
		mtv,
		{ minkowskiHull.GetContactPoint() },
		mtv.GetNormalized(),
		otherNormal,
		DetailedCollisionInfo::MtvDirection::A_TO_B);
}
