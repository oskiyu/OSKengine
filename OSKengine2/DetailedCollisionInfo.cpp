#include "DetailedCollisionInfo.h"

using namespace OSK;
using namespace OSK::COLLISION;

DetailedCollisionInfo DetailedCollisionInfo::True(Vector3f minimumTranslationVector, const DynamicArray<Vector3f>& points) {
	DetailedCollisionInfo output{};
	output.isColliding = true;
	output.minimumTranslationVector = minimumTranslationVector;
	output.contactPoints = points;

	output.singleContactPoint = 0.0f;
	for (const auto& point : points)
		output.singleContactPoint += point;
	output.singleContactPoint /= points.GetSize();

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
