#include "Clipping.h"

OSK::DynamicArray<OSK::Vector3f> OSK::COLLISION::ClipFaces(const DynamicArray<Vector3f>& referenceFace, const DynamicArray<Vector3f>& finalFace, const Vector3f& firstFaceNormal, const Vector3f& secondFaceNormal) {
	DynamicArray<Vector3f> intermediatePoints = DynamicArray<Vector3f>(finalFace);

	const Vector3f firstCenter = GetCenter(intermediatePoints);

	for (UIndex64 v = 0; v < referenceFace.GetSize(); v++) {
		const Vector3f& a = referenceFace[v];
		const Vector3f& b = referenceFace[(v + 1) % referenceFace.GetSize()];

		// L�nea formada por los dos v�rtices.
		const Vector3f ab = (b - a).GetNormalized();

		// Direcci�n perpendicular a la l�nea.
		// Direcci�n de clipping.
		const Vector3f lineDirection = ab.Cross(firstFaceNormal).GetNormalized();

		const float centerProjection = firstCenter.Dot(lineDirection);
		const float vertexProjection = a.Dot(lineDirection);

		// Si apunta hacia el centro (dot >= 0.0f), se debe invertir la direcci�n.
		const float sign = glm::sign(centerProjection - vertexProjection) >= 0.0f ? -1.0f : 1.0f;

		// Direcci�n de clipping final.
		const Vector3f direction = lineDirection * sign;

		const float referenceProjection = a.Dot(direction);

		for (auto& point : intermediatePoints) {

			// Proyecci�n del punto sobre la direcci�n de clipping.
			const float pDot = point.Dot(direction);

			// Diferencia entre la proyecci�n del punto y de referencia.
			const float diff = pDot - referenceProjection;

			// Si la diferencia es mayor que 0, entonces el punto est� mas all� del
			// plano, y debemos traerlo.
			if (diff > 0.0f)
				point -= direction * diff;
		}
	}

	DynamicArray<Vector3f> finalContactPoints = DynamicArray<Vector3f>::CreateReservedArray(intermediatePoints.GetSize());
	const float referenceDistance = referenceFace[0].Dot(-firstFaceNormal) - 0.01f;

	for (const Vector3f& point : intermediatePoints) {
		const float distance = point.Dot(-firstFaceNormal);

		if (distance > referenceDistance) {
			finalContactPoints.Insert(point);
		}
	}

	return finalContactPoints;
}

static OSK::Vector3f OSK::COLLISION::GetCenter(const DynamicArray<Vector3f>& face) {
	Vector3f output = Vector3f::Zero;

	for (const auto& v : face) {
		output += v;
	}

	return output / static_cast<float>(face.GetSize());
}
