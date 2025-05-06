#include "FullMinkowskiHull.h"

#include "ConvexVolume.h"

#include "OSKengine.h"
#include "Logger.h"

#include "InvalidObjectStateException.h"

#include "Math.h"

using namespace OSK;
using namespace OSK::COLLISION;


FullMinkowskiHull FullMinkowskiHull::BuildFromVolumes(const ConvexVolume& first, const ConvexVolume& second) {
	FullMinkowskiHull output{};
	output.BuildFullHull(first, second);

	return output;
}

void FullMinkowskiHull::BuildFullHull(const ConvexVolume& first, const ConvexVolume& second) {
	const auto& firstVertices = first.GetVertices();
	const auto& secondVertices = second.GetVertices();

	const auto& firstAxes = first.GetAxes();
	const auto& secondAxes = second.GetAxes();

	const auto& firstFaces = first.GetFaceIndices();
	const auto& secondFaces = second.GetFaceIndices();

	float minDistance = std::numeric_limits<float>::max();

	m_faces.Insert({ 0, 1, 2 });
	m_vertices.InsertAll({ {},{},{} });

	for (UIndex64 i = 0; i < firstAxes.GetSize(); i++) {
		const auto& axis = firstAxes[i];
		const auto& refVertex = firstVertices[firstFaces[i][0]];

		const auto secondSupport = second.GetSupport(-axis);

		const float positiveProjection = refVertex.Dot(axis);
		const float negativeProjection = secondSupport.point.Dot(axis);

		const float minkowskiProjection = positiveProjection - negativeProjection;

		if (minkowskiProjection < minDistance) {
			minDistance = minkowskiProjection;

			m_mtv = axis * minkowskiProjection;

			m_vertices[0].originalVertexIdA = firstFaces[i][0];
			m_vertices[0].originalVertexIdB = secondSupport.originalVertexId;

			m_vertices[1].originalVertexIdA = firstFaces[i][1];
			m_vertices[1].originalVertexIdB = secondSupport.originalVertexId;

			m_vertices[2].originalVertexIdA = firstFaces[i][2];
			m_vertices[2].originalVertexIdB = secondSupport.originalVertexId;

			m_referenceVolume = Volume::A;
		}
	}

	for (UIndex64 i = 0; i < secondAxes.GetSize(); i++) {
		const auto& axis = secondAxes[i];
		const auto& refVertex = secondVertices[secondFaces[i][0]];

		const auto firstSupport = first.GetSupport(-axis);

		const float positiveProjection = refVertex.Dot(axis);
		const float negativeProjection = firstSupport.point.Dot(axis);

		const float minkowskiProjection = positiveProjection - negativeProjection;

		if (minkowskiProjection < minDistance) {
			minDistance = minkowskiProjection;

			m_mtv = -axis * minkowskiProjection;

			m_vertices[0].originalVertexIdA = firstSupport.originalVertexId;
			m_vertices[0].originalVertexIdB = secondFaces[i][0];

			m_vertices[1].originalVertexIdA = firstSupport.originalVertexId;
			m_vertices[1].originalVertexIdB = secondFaces[i][1];

			m_vertices[2].originalVertexIdA = firstSupport.originalVertexId;
			m_vertices[2].originalVertexIdB = secondFaces[i][2];

			m_referenceVolume = Volume::B;
		}
	}
}

bool FullMinkowskiHull::SurpasesMinDepth() const {
	return m_surpassesMinDepth;
}

Vector3f FullMinkowskiHull::GetMtv() const {
	return m_mtv;
}

Vector3f FullMinkowskiHull::GetNormalizedMtv() const {
	return m_mtv.GetNormalized();
}

std::array<UIndex64, 3> FullMinkowskiHull::GetFirstFaceVerticesIds() const {
	const Face& face = m_faces[0];

	const MinkowskiSupport& a = m_vertices[face[0]];
	const MinkowskiSupport& b = m_vertices[face[1]];
	const MinkowskiSupport& c = m_vertices[face[2]];

	return { a.originalVertexIdA.value(), b.originalVertexIdA.value(), c.originalVertexIdA.value() };
}

std::array<UIndex64, 3> FullMinkowskiHull::GetSecondFaceVerticesIds() const {
	const Face& face = m_faces[0];

	const MinkowskiSupport& a = m_vertices[face[0]];
	const MinkowskiSupport& b = m_vertices[face[1]];
	const MinkowskiSupport& c = m_vertices[face[2]];

	return { a.originalVertexIdB.value(), b.originalVertexIdB.value(), c.originalVertexIdB.value() };
}

FullMinkowskiHull::Volume FullMinkowskiHull::GetReferenceId() const {
	return m_referenceVolume;
}
