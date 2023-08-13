#include "DetailedCollisionInfo.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

DetailedCollisionInfo DetailedCollisionInfo::True(
	Vector3f minimumTranslationVector,
	const DynamicArray<Vector3f>& points,
	Vector3f firstFaceNormal,
	Vector3f secondFaceNormal,
	bool shouldSwaphObjects) {

	DetailedCollisionInfo output{};
	output.isColliding = true;
	output.minimumTranslationVector = minimumTranslationVector;
	output.contactPoints = points;
	output.swapObjects = shouldSwaphObjects;
	output.firstFaceNormal = firstFaceNormal.GetNormalized();
	output.secondFaceNormal = secondFaceNormal.GetNormalized();

	output.singleContactPoint = 0.0f;
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

bool DetailedCollisionInfo::ShouldSwapObjects() const {
	return swapObjects;
}
