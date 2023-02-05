// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "ConvexVolume.h"

#include "Transform3D.h"
#include "FaceProjection.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

void ConvexVolume::AddFace(const DynamicArray<Vector3f>& points) {
	const TSize numOldVertices = vertices.GetSize();
	const TSize numNewVertices = points.GetSize();

	FaceIndices indices = FaceIndices::CreateReservedArray(numNewVertices);
	for (TIndex i = 0; i < numNewVertices; i++) {
		indices.Insert(numOldVertices + i);
		vertices.Insert(points[i]);
	}

	faces.Insert(indices);
}

void ConvexVolume::OptimizeAxes() {
	if (faces.IsEmpty())
		return;

	DynamicArray<FaceIndices> optimizedFaces = {};
	DynamicArray<TIndex> duplicatedIds = {};

	for (TIndex i = 0; i < faces.GetSize() - 1; i++) {

		// Por cada eje original, comprobamos si hay otros
		// ejes equivalentes.

		for (TIndex j = i + 1; j < faces.GetSize(); j++) {

			const Axis axis1 = GetLocalSpaceAxis(i);
			const Axis axis2 = GetLocalSpaceAxis(j);

			// @todo epsilon?
			const bool isSameAxis = axis1 == axis2 || axis1 == -axis2;

			// Si el otro eje es igual, lo marcamos para no añadirlo
			// a la lista optimizada.
			if (isSameAxis)
				duplicatedIds.Insert(j);
		}
	}

	// Añadimos las caras que no hayan sido marcadas.
	for (TIndex i = 0; i < faces.GetSize(); i++)
		if (!duplicatedIds.ContainsElement(i))
			optimizedFaces.Insert(faces[i]);

	faces = optimizedFaces;
}

ConvexVolume::Axis ConvexVolume::GetLocalSpaceAxis(TIndex faceId) const {
	const FaceIndices& faceIndices = faces[faceId];

	const Vector3f pointA = vertices.At(faceIndices.At(0));
	const Vector3f pointB = vertices.At(faceIndices.At(1));
	const Vector3f pointC = vertices.At(faceIndices.At(2));

	const Vector3f vec1 = pointB - pointA;
	const Vector3f vec2 = pointC - pointA;

	return vec1.Cross(vec2).GetNormalized();
}

ConvexVolume::Axis ConvexVolume::GetWorldSpaceAxis(TIndex faceId, const Transform3D& transform) const {
	const FaceIndices& faceIndices = faces[faceId];

	const Vector3f pointA = vertices.At(faceIndices.At(0));
	const Vector3f pointB = vertices.At(faceIndices.At(1));
	const Vector3f pointC = vertices.At(faceIndices.At(2));

	const Vector3f transformedA = transform.TransformPoint(pointA);
	const Vector3f transformedB = transform.TransformPoint(pointB);
	const Vector3f transformedC = transform.TransformPoint(pointC);

	const Vector3f vec1 = transformedB - transformedA;
	const Vector3f vec2 = transformedC - transformedA;

	return vec1.Cross(vec2).GetNormalized();
}

DetailedCollisionInfo ConvexVolume::GetCollisionInfo(const IBottomLevelCollider& other,
	const Transform3D& thisTransform, const Transform3D& otherTransform) const {

	const ConvexVolume& otherVolume = (const ConvexVolume&)other;

	// Número de caras (y por lo tanto, de ejes) de ambos colliders.
	const TSize thisFaceCount = this->faces.GetSize();
	const TSize otherFaceCount = otherVolume.faces.GetSize();

	// Vértices en world-space de cada collider.
	const auto transformedVerticesA = this->GetWorldSpaceVertices(thisTransform);
	const auto transformedVerticesB = otherVolume.GetWorldSpaceVertices(otherTransform);

	// Vector mínimo que se debe aplicar para separar las entidades,
	// de tal manera que dejen de estar en colisión.

	Vector3f mtv = 0.0f;

	// Overlap del mtv.
	float minimumOverlap = std::numeric_limits<float>::max();

	// Comprobamos los ejes de este collider.
	for (TIndex i = 0; i < thisFaceCount; i++) {
		const Axis axis = this->GetWorldSpaceAxis(i, thisTransform);

		const FaceProjection projA = FaceProjection(transformedVerticesA, axis);
		const FaceProjection projB = FaceProjection(transformedVerticesB, axis);

		if (!projA.Overlaps(projB))
			return DetailedCollisionInfo::False();

		const float overlap = projA.GetOverlap(projB);
		if (glm::abs(overlap) < glm::abs(minimumOverlap)) {
			minimumOverlap = overlap;
			mtv = axis;
		}
	}

	// Comprobamos los ejes del otro collider.
	for (TIndex i = 0; i < otherFaceCount; i++) {
		const Axis axis = otherVolume.GetWorldSpaceAxis(i, otherTransform);

		const FaceProjection projA = FaceProjection(transformedVerticesA, axis);
		const FaceProjection projB = FaceProjection(transformedVerticesB, axis);

		if (!projA.Overlaps(projB))
			return DetailedCollisionInfo::False();

		const float overlap = projA.GetOverlap(projB);
		if (glm::abs(overlap) < glm::abs(minimumOverlap)) {
			minimumOverlap = overlap;
			mtv = axis;
		}
	}

	return DetailedCollisionInfo::True(mtv.GetNormalized() * minimumOverlap);
}

bool ConvexVolume::ContainsPoint(const Vector3f& point) const {
	OSK_ASSERT(false, "No implementado");
	return false;
}

DynamicArray<Vector3f> ConvexVolume::GetWorldSpaceVertices(const Transform3D& transform) const {
	DynamicArray<Vector3f> output;

	for (const auto& vertex : vertices)
		output.Insert(transform.TransformPoint(vertex));

	return output;
}

const DynamicArray<Vector3f>& ConvexVolume::GetLocalSpaceVertices() const {
	return vertices;
}

const DynamicArray<ConvexVolume::FaceIndices> ConvexVolume::GetFaceIndices() const {
	return faces;
}
