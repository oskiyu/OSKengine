// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "CollisionInfo.h"

using namespace OSK;
using namespace OSK::ECS;
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

CollisionInfo CollisionInfo::True(const DynamicArray<DetailedCollisionInfo>& dInfo) {
	CollisionInfo info(true, true);
	info.detailedInfo = dInfo;

	return info;
}

bool CollisionInfo::IsColliding() const {
	return isColliding;
}

bool CollisionInfo::IsTopLevelColliding() const {
	return isTopLevelColliding;
}

const DynamicArray<DetailedCollisionInfo>& CollisionInfo::GetDetailedInfo() const {
	return detailedInfo;
}
