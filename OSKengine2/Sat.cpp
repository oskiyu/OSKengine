#include "Sat.h"

OSK::COLLISION::SatInfo OSK::COLLISION::SatCollision(const ISatCollider& first, const ISatCollider& second) {
	SatInfo output{};
	output.overlaps = true;
	output.mtv = Vector3f::Zero;
	output.face.index = 0;
	output.face.collider = SatInfo::Face::Collider::A;

	const auto& firstAxes = first.GetAxes();
	const auto& secondAxes = second.GetAxes();

	const auto& firstVertices = first.GetVertices();
	const auto& secondVertices = second.GetVertices();

	const USize64 axesCount = firstAxes.GetSize() + secondAxes.GetSize();

	float minimumOverlap = std::numeric_limits<float>::max();

	for (UIndex32 faceIndex = 0; faceIndex < axesCount; faceIndex++) {
		const bool isFirstCollider = faceIndex < firstAxes.GetSize();
		const UIndex64 finalIndex = isFirstCollider ? faceIndex : faceIndex - firstAxes.GetSize();

		const Vector3f axis = isFirstCollider
			? firstAxes[finalIndex]
			: secondAxes[finalIndex];

		const auto projA = FaceProjection(firstVertices, axis);
		const auto projB = FaceProjection(secondVertices, axis);

		// Si no hay overlap, no hay colisión.
		if (!projA.Overlaps(projB)) {
			output.overlaps = false;

			return output;
		}


		// Distancia que los dos objetos se solapan en este eje.
		const float overlap = projA.GetOverlap(projB);

		if (overlap < minimumOverlap) {
			minimumOverlap = overlap;
			output.overlaps = true;
			output.mtv = axis * overlap;
			output.face.index = finalIndex;
			output.face.collider = isFirstCollider ? SatInfo::Face::Collider::A : SatInfo::Face::Collider::B;
		}
	}

	return output;
}
