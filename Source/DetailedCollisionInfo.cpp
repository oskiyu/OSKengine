#include "DetailedCollisionInfo.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

DetailedCollisionInfo DetailedCollisionInfo::True(
	Vector3f minimumTranslationVector,
	const DynamicArray<Vector3f>& points,
	Vector3f firstFaceNormal,
	Vector3f secondFaceNormal,
	MtvDirection mtvDirection) {

	DetailedCollisionInfo output{};
	output.isColliding = true;
	output.minimumTranslationVector = minimumTranslationVector;
	output.contactPoints = DynamicArray<Vector3f>::CreateReserved(points.GetSize());
	output.mtvDirection = mtvDirection;
	output.firstFaceNormal = firstFaceNormal.GetNormalized();
	output.secondFaceNormal = secondFaceNormal.GetNormalized();
	
	for (const auto& v : points) {
		bool containsVertex = false;
		for (const auto& v2 : output.contactPoints) {
			if (v.Equals(v2, std::numeric_limits<float>::epsilon() * 100.0f)) {
				containsVertex = true;
				continue;
			}
		}

		if (!containsVertex) {
			output.contactPoints.Insert(v);
		}
	}

	output.singleContactPoint = Vector3f::Zero;
	for (const auto& point : points)
		output.singleContactPoint += point;
	output.singleContactPoint /= static_cast<float>(points.GetSize());

	return output;
}

DetailedCollisionInfo DetailedCollisionInfo::False() {
	DetailedCollisionInfo output{};
	output.isColliding = false;

	return output;
}

bool DetailedCollisionInfo::IsColliding() const {
	return isColliding;
}

Vector3f DetailedCollisionInfo::GetMinimumTranslationVector() const {
	return minimumTranslationVector;
}

const DynamicArray<Vector3f>& DetailedCollisionInfo::GetContactPoints() const {
	return contactPoints;
}

Vector3f DetailedCollisionInfo::GetSingleContactPoint() const {
	return singleContactPoint;
}

Vector3f DetailedCollisionInfo::GetFirstFaceNormal() const {
	return firstFaceNormal;
}

Vector3f DetailedCollisionInfo::GetSecondFaceNormal() const {
	return secondFaceNormal;
}

DetailedCollisionInfo::MtvDirection DetailedCollisionInfo::GetMtvDirection() const {
	return mtvDirection;
}
