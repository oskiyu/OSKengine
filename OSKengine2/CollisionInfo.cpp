// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "CollisionInfo.h"

using namespace OSK;
using namespace OSK::COLLISION;

CollisionInfo::CollisionInfo(bool isColliding, bool isTopLevelColliding)
	: isColliding(isColliding), isTopLevelColliding(isTopLevelColliding) {

}

CollisionInfo CollisionInfo::False() {
	return CollisionInfo(false, false);
}

CollisionInfo CollisionInfo::TopLevelOnly() {
	return CollisionInfo(false, true);
}

CollisionInfo CollisionInfo::True() {
	return CollisionInfo(true, true);
}

bool CollisionInfo::IsColliding() const {
	return isColliding;
}

bool CollisionInfo::IsTopLevelColliding() const {
	return isTopLevelColliding;
}
