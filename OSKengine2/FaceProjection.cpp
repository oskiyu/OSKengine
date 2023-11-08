// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "FaceProjection.h"

using namespace OSK;
using namespace OSK::COLLISION;

FaceProjection::FaceProjection(const DynamicArray<Vector3f>& vertices, Vector3f axis) {
	for (const auto& vertex : vertices) {
		const float vertexProjection = vertex.Dot(axis);

		min = glm::min(vertexProjection, min);
		max = glm::max(vertexProjection, max);
	}
}

bool FaceProjection::Overlaps(FaceProjection other) const {
	return this->max > other.min && this->min < other.max;
}

float FaceProjection::GetOverlap(FaceProjection other) const {
	return glm::max(0.0f,
		glm::min(max, other.max) - glm::max(min, other.min)
	);

	if (!Overlaps(other))
		return 0.0f;

	// Esta proyección está dentro de la otra.
	if (this->min < other.min && this->max > other.max)
		return glm::abs(this->max - this->min);

	// La otra proyección está dentro de esta.
	if (other.min < this->min && other.max > this->max)
		return glm::abs(other.max - other.min);

	return glm::max(
			glm::abs(this->min - other.max),
			glm::abs(this->max - other.min)
		);
}

float FaceProjection::GetMin() const {
	return min;
}

float FaceProjection::GetMax() const {
	return max;
}
