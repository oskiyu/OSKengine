// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "IBottomLevelCollider.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

void IBottomLevelCollider::SetTransformOffset(const Transform3D& offset) {
	this->transformOffset = offset;
}

const Transform3D& IBottomLevelCollider::GetTransformOffset() const {
	return transformOffset;
}
