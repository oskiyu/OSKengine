#include "GjkClippingDetector.h"

#include "Gjk.h"
#include "Simplex.h"
#include "FullMinkowskiHull.h"
#include "NarrowColliderHolder.h"
#include "ConvexVolume.h"
#include "Clipping.h"

std::optional<OSK::COLLISION::DetailedCollisionInfo> OSK::COLLISION::GjkClippingDetector::GetCollision(
	const NarrowColliderHolder& first,
	const NarrowColliderHolder& second) const
{
	if (!(first.GetCollider()->Is<ConvexVolume>() && second.GetCollider()->Is<ConvexVolume>())) {
		return {};
	}

	const auto& castedFirst  = *first.GetCollider()->As<ConvexVolume>();
	const auto& castedSecond = *second.GetCollider()->As<ConvexVolume>();

	const Simplex simplex = Simplex::GenerateFrom(castedFirst, castedSecond);

	if (!simplex.ContainsOrigin()) {
		return DetailedCollisionInfo::False();
	}

	const auto minkowskiHull = FullMinkowskiHull::BuildFromVolumes(castedFirst, castedSecond);

	if (!minkowskiHull.SurpasesMinDepth()) {
		return DetailedCollisionInfo::False();
	}

	Vector3f mtv = minkowskiHull.GetMtv();

	const UIndex64 firstFaceId  = castedFirst.GetFaceWithVertexIndices(minkowskiHull.GetFirstFaceVerticesIds(), minkowskiHull.GetMtv());
	const UIndex64 secondFaceId = castedSecond.GetFaceWithVertexIndices(minkowskiHull.GetSecondFaceVerticesIds(), -minkowskiHull.GetMtv());

	const ConvexVolume::FaceIndices& faceA = castedFirst .GetFaceIndices()[firstFaceId];
	const ConvexVolume::FaceIndices& faceB = castedSecond.GetFaceIndices()[secondFaceId];

	DynamicArray<Vector3f> verticesA = DynamicArray<Vector3f>::CreateReserved(faceA.GetSize());
	for (const auto index : faceA) {
		verticesA.Insert(castedFirst.GetVertices()[index]);
	}

	DynamicArray<Vector3f> verticesB = DynamicArray<Vector3f>::CreateReserved(faceB.GetSize());
	for (const auto index : faceB) {
		verticesB.Insert(castedSecond.GetVertices()[index]);
	}

	// Get most incident face:
	FullMinkowskiHull::Volume incident = FullMinkowskiHull::Volume::A;
	{
		const float firstDot  = castedFirst .GetAxes()[firstFaceId] .Dot(-mtv);
		const float secondDot = castedSecond.GetAxes()[secondFaceId].Dot(mtv);

		incident = firstDot < secondDot ? FullMinkowskiHull::Volume::A : FullMinkowskiHull::Volume::B;
	}

	const Vector3f normal	   = minkowskiHull.GetNormalizedMtv();
	const Vector3f otherNormal = castedSecond.GetWorldSpaceAxis(secondFaceId);

	const DynamicArray<Vector3f> finalContactPoints = incident == FullMinkowskiHull::Volume::A
		? ClipFaces(verticesA, verticesB, normal, otherNormal)
		: ClipFaces(verticesB, verticesA, otherNormal, normal);


	return DetailedCollisionInfo::True(
		mtv,
		finalContactPoints,
		minkowskiHull.GetNormalizedMtv(),
		otherNormal,
		DetailedCollisionInfo::MtvDirection::A_TO_B);
}
