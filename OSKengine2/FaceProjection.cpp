// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "FaceProjection.h"

using namespace OSK;
using namespace OSK::COLLISION;

FaceProjection::FaceProjection(const DynamicArray<Vector3f>& vertices, Vector3f axis) {
	for (const auto& vertex : vertices) {
		const float vertexProjection = axis.Dot(vertex);

		if (vertexProjection < min)
			min = vertexProjection;

		if (vertexProjection > max)
			max = vertexProjection;
	}
}

bool FaceProjection::Overlaps(FaceProjection other) const {
	return max > other.min && min < other.max;
}

float FaceProjection::GetOverlap(FaceProjection other) const {
	if (!Overlaps(other))
		return 0.0f;

	float output = 0.0f;
	output = max - other.min;

	const float out = min - other.max;
	if (glm::abs(out) < glm::abs(output))
		output = out;

	return output;
}

float FaceProjection::GetMin() const {
	return min;
}

float FaceProjection::GetMax() const {
	return max;
}
