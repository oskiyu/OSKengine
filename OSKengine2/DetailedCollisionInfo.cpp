#include "DetailedCollisionInfo.h"

using namespace OSK;
using namespace OSK::COLLISION;

DetailedCollisionInfo DetailedCollisionInfo::True(Vector3f minimumTranslationVector) {
	DetailedCollisionInfo output{};
	output.isColliding = true;
	output.minimumTranslationVector = minimumTranslationVector;

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
