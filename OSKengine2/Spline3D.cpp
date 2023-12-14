#include "Spline3D.h"

using namespace OSK;

void Spline3D::AddPoint(const Vector3f& point) {
	m_points.Insert(point);
}

USize64 Spline3D::GetPointCount() const {
	return m_points.GetSize();
}

const Vector3f& Spline3D::GetPoint(UIndex64 i) const {
	return m_points[i % m_points.GetSize()];
}

void Spline3D::Normalize() {
	if (m_points.IsEmpty()) {
		return;
	}

	DynamicArray<Vector3f> finalPoints = DynamicArray<Vector3f>::CreateReservedArray(m_points.GetSize());
	finalPoints.Insert(m_points[0]);

	for (UIndex64 i = 0; i < m_points.GetSize(); i++) {
		UIndex64 j = (i + 1) % m_points.GetSize();

		const Vector3f pointA = m_points[i];
		const Vector3f pointB = m_points[j];

		if (!pointA.Equals(pointB, 0.0001f)) {
			finalPoints.Insert(pointB);
		}
	}

	m_points = finalPoints;
}

UIndex64 Spline3D::GetClosestPointIndex(const Vector3f& position) const {
	UIndex64 output = 0;
	float minDistance2 = std::numeric_limits<float>::max();

	for (UIndex64 i = 0; i < m_points.GetSize(); i++) {
		const float distance2 = m_points[i].GetDistanceTo2(position);

		if (distance2 < minDistance2) {
			minDistance2 = distance2;
			output = i;
		}
	}

	return output;
}

UIndex64 Spline3D::GetClosestForwardPointIndex(const Vector3f& position, const Vector3f& direction) const {
	UIndex64 output = 0;
	float minDistance2 = std::numeric_limits<float>::max();

	for (UIndex64 i = 0; i < m_points.GetSize(); i++) {
		const Vector3f& current = m_points[i];
		const Vector3f& next = m_points[(i + 1) % m_points.GetSize()];

		const float dot = (next - current).Dot(direction);

		if (dot < 0.0f) {
			continue;
		}

		const float distance2 = m_points[i].GetDistanceTo2(position);

		if (distance2 < minDistance2) {
			minDistance2 = distance2;
			output = i;
		}
	}

	return output;
}

const Vector3f& Spline3D::GetClosestPoint(const Vector3f& position) const {
	return m_points[GetClosestPointIndex(position)];
}

const Vector3f& Spline3D::GetClosestForwardPoint(const Vector3f& position, const Vector3f& direction) const {
	return m_points[GetClosestForwardPointIndex(position, direction)];
}
