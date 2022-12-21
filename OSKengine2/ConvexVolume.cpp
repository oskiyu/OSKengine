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

			const Axis axis1 = GetAxisFromFace(i);
			const Axis axis2 = GetAxisFromFace(j);

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

ConvexVolume::Axis ConvexVolume::GetAxisFromFace(TIndex faceId) const {
	const FaceIndices& faceIndices = faces[faceId];
	const Vector3f pointA = vertices.At(faceIndices.At(0));
	const Vector3f pointB = vertices.At(faceIndices.At(1));
	const Vector3f pointC = vertices.At(faceIndices.At(2));

	const Vector3f vec1 = pointB - pointA;
	const Vector3f vec2 = pointC - pointA;

	return vec1.Cross(vec2).GetNormalized();
}

ConvexVolume::Axis ConvexVolume::GetAxisFromFace(TIndex faceId, const Transform3D& transform) const {
	const static glm::vec4 positionMult = { 0.0f, 0.0f, 0.0f, 1.0f };

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

bool ConvexVolume::IsColliding(const IBottomLevelCollider& other,
	const Transform3D& thisTransform, const Transform3D& otherTransform) const {

	const ConvexVolume& otherVolume = (const ConvexVolume&)other;

	const TSize thisFaceCount = faces.GetSize();
	const TSize otherFaceCount = otherVolume.faces.GetSize();

	const auto transformedVerticesA = GetTransformedVertices(thisTransform);
	const auto transformedVerticesB = otherVolume.GetTransformedVertices(otherTransform);

	for (TIndex i = 0; i < thisFaceCount; i++) {
		const Axis axis = GetAxisFromFace(i, thisTransform);

		const FaceProjection projA = FaceProjection(transformedVerticesA, axis);
		const FaceProjection projB = FaceProjection(transformedVerticesB, axis);

		if (!projA.Overlaps(projB))
			return false;
	}

	for (TIndex i = 0; i < otherFaceCount; i++) {
		const Axis axis = GetAxisFromFace(i, thisTransform);

		const FaceProjection projA = FaceProjection(transformedVerticesA, axis);
		const FaceProjection projB = FaceProjection(transformedVerticesB, axis);

		if (!projA.Overlaps(projB))
			return false;
	}

	return true;
}

DynamicArray<Vector3f> ConvexVolume::GetTransformedVertices(const Transform3D& transform) const {
	DynamicArray<Vector3f> output;

	for (const auto& vertex : vertices)
		output.Insert(transform.TransformPoint(vertex));

	return output;
}
