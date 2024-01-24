#include "MinkowskiHull.h"

#include "ConvexVolume.h"

#include "OSKengine.h"
#include "Logger.h"

#include "InvalidObjectStateException.h"

#include "Math.h"

using namespace OSK;
using namespace OSK::COLLISION;


MinkowskiHull MinkowskiHull::BuildFromVolumes(const ConvexVolume& first, const ConvexVolume& second) {
	MinkowskiHull output{};
	output.BuildFullHull(first, second);

	return output;
}

void MinkowskiHull::BuildFullHull(const ConvexVolume& first, const ConvexVolume& second) {
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

#ifdef OSK_EPA_IMPL
Polytope Polytope::FromSimplex(const Simplex& simplex) {
	return Polytope(simplex);
}

Polytope::Polytope(const Simplex& simplex) {
	const auto& simplexVertices = simplex.GetVertices();

	for (UIndex64 i = 0; i < simplex.GetVerticesCount(); i++) {
		m_vertices.Insert(simplexVertices[i]);

		if (simplexVertices[i].point.Equals(Vector3d::Zero, MinDistanceThreshold)) {
			m_surpassesMinDepth = false;
			return;
		}
	}

	m_faces = {
		{ 0, 1, 2 },
		{ 0, 3, 1 },
		{ 0, 2, 3 },
		{ 1, 3, 2 }
	};

	Update();
}


void Polytope::Expand(const ConvexVolume& first, const ConvexVolume& second) {
#define OSK_EPA_DEBUG
	constexpr static USize64 numIterations = 24;

	UIndex64 iteration = 0;
	for (; iteration < numIterations; iteration++) {
		Update();

		if (!m_surpassesMinDepth) {
			break;
		}

		if (m_earlyExit) {
#ifdef OSK_EPA_DEBUG
			Engine::GetLogger()->InfoLog("\tSALIDA: m_earlyExit (NaN)");
#endif

			break;
		}


		const double currentDistance = m_distances[m_minDistanceIndex];
		const Vector3d currentNormal = m_normals[m_minDistanceIndex];


		// Si la distancia está ya por debajo de un número bajo,
		// lo consideramos válido.
		if (currentDistance < MinDistanceThreshold) {
#ifdef OSK_EPA_DEBUG
			Engine::GetLogger()->InfoLog("\tSALIDA: currentDistance < MinDistanceThreshold");
#endif
			m_surpassesMinDepth = false;

			break;
		}

#ifdef OSK_EPA_DEBUG
		Engine::GetLogger()->DebugLog(std::format("Iteración {}", iteration));
		Engine::GetLogger()->DebugLog(std::format("\tNormal mínimo: {:.3f} {:.3f} {:.3f}", currentNormal.x, currentNormal.y, currentNormal.z));
		Engine::GetLogger()->DebugLog(std::format("\tDistancia mínima: {:.3f}", m_distances[m_minDistanceIndex]));
#endif

		// Punto del hull más hacia afuera en la dirección
		// actual (normal de la cara con distancia mínima).
		MinkowskiSupport support = GetMinkowskiSupport(first, second, currentNormal);

		// Punto actual más hacia afuera en la dirección actual.
		const Vector3d furthestPoint = this->GetSupport(currentNormal);

#ifdef OSK_EPA_DEBUG
		Engine::GetLogger()->DebugLog(std::format("\tSoporte teórico: {:.3f} {:.3f} {:.3f}", support.point.x, support.point.y, support.point.z));
		Engine::GetLogger()->DebugLog(std::format("\tSoporte actual: {:.3f} {:.3f} {:.3f}", furthestPoint.x, furthestPoint.y, furthestPoint.z));
#endif


		// Distancia del soporte respecto al origen en la dirección actual.
		const float supportProjection = support.point.Dot(currentNormal);

#ifdef OSK_EPA_DEBUG
		Engine::GetLogger()->DebugLog(std::format("\tDistancia teórica (proj): {:.3f}", supportProjection));
#endif

		// Diferencia entre las distancias nueva y actual.
		const float newDistanceDiff = glm::abs(currentDistance - supportProjection);
		bool alreadyContainsPoint = false;
		for (const auto& vertex : m_vertices) {
			if (vertex.point.Equals(support.point, MinDistanceThreshold)) {
				alreadyContainsPoint = true;
				break;
			}
		}


		if (newDistanceDiff < MinDistanceThreshold) {
#ifdef OSK_EPA_DEBUG
			Engine::GetLogger()->InfoLog("\tSALIDA: newDistanceDiff < MinDistanceThreshold");
#endif

			break;
		}

		if (alreadyContainsPoint) {
#ifdef OSK_EPA_DEBUG
			Engine::GetLogger()->InfoLog("\tSALIDA: alreadyContainsPoint");
#endif

			break;
		}


		// Aristas que forma el borde de las caras que están
		// mirando hacia el nuevo punto.
		DynamicArray<Edge> uniqueEdges;

		// ELiminamos las caras que estén mirando hacia el nuevo punto.
		for (int64_t faceId = 0; faceId < m_faces.GetSize(); faceId++) {
			const auto& face = m_faces[faceId];
			const auto vertexIndex = face[0];

			const Vector3d faceVertex = m_vertices[vertexIndex].point;
			const Vector3d faceNormal = m_normals[faceId];

			const double faceVertexDot = faceVertex.Dot(faceNormal);
			const double supportDot = support.point.Dot(faceNormal);

			const bool isSameDirection = supportDot > faceVertexDot;

			// Si la cara no apunta hacia el nuevo punto,
			// no hace falta hacer nada.
			if (!isSameDirection) {
				continue;
			}

			AddUniqueEdge(&uniqueEdges, face[0], face[1]);
			AddUniqueEdge(&uniqueEdges, face[1], face[2]);
			AddUniqueEdge(&uniqueEdges, face[2], face[0]);

			// Eliminamos la cara.
			m_faces[faceId] = m_faces.Pop();
			m_normals[faceId] = m_normals.Pop();
			m_distances[faceId] = m_distances.Pop();

			if (faceId == m_minDistanceIndex) {
				m_minDistanceIndex = std::numeric_limits<UIndex64>::max();
			}

			faceId--;
		}

		// Nuevas caras construidas a partir de las aristas
		// y el nuevo vértice.
		m_faces.InsertAll(BuildNewFaces(uniqueEdges, &m_vertices, support));
	}

#ifdef OSK_EPA_DEBUG
	Engine::GetLogger()->InfoLog(std::format("\tNum. final de iteraciones: {}", iteration));
	if (GetMtv().GetLenght() > 0.08f) {
		Engine::GetLogger()->InfoLog("\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}
#endif // OSK_EPA_DEBUG
}

void Polytope::Update() {
	// Construir nuevos normales:
	for (UIndex64 i = m_normals.GetSize(); i < m_faces.GetSize(); i++) {
		const auto& face = m_faces[i];

		const Vector3d& a = m_vertices[face[0]].point;
		const Vector3d& b = m_vertices[face[1]].point;
		const Vector3d& c = m_vertices[face[2]].point;

		const Vector3d ab = b - a;
		const Vector3d ac = c - a;

		m_normals.Insert(ab.Cross(ac).GetNormalized());

		if (m_normals[i].IsNaN()) {
			m_earlyExit = true;
			return;
		}

		m_distances.Insert(a.Dot(m_normals[i]));
		
		OSK_ASSERT(!m_normals[i].IsNaN(), InvalidObjectStateException("Vector NAN"));

		if (m_distances[i] < 0.0f) {
			m_normals[i] *= -1.0f;
			m_distances[i] *= -1.0f;
		}

	}

	double minDistance = std::numeric_limits<double>::max();
	for (UIndex64 i = 0; i < m_distances.GetSize(); i++) {
		if (m_distances[i] < minDistance) {
			minDistance = m_distances[i];
			m_minDistanceIndex = i;
		}
	}
}

void Polytope::AddUniqueEdge(DynamicArray<Edge>* edges, UIndex64 vertexA, UIndex64 vertexB) {
	const Edge normalEdge = { .a = vertexA, .b = vertexB };
	const Edge inversedEdge = { .a = vertexB, .b = vertexA };

	const UIndex64 normalEdgeIndex = edges->GetIndexOf(normalEdge);
	const UIndex64 reveresedEdgeIndex = edges->GetIndexOf(inversedEdge);

	if (normalEdgeIndex != DynamicArray<Polytope::Edge>::NotFoundIndex) {
		edges->RemoveAt(normalEdgeIndex);
	}
	if (reveresedEdgeIndex != DynamicArray<Polytope::Edge>::NotFoundIndex) {
		edges->RemoveAt(reveresedEdgeIndex);
	}

	if (normalEdgeIndex == DynamicArray<Polytope::Edge>::NotFoundIndex && reveresedEdgeIndex == DynamicArray<Polytope::Edge>::NotFoundIndex) {
		edges->Insert({ vertexA, vertexB });
	}
}

DynamicArray<Polytope::Face> Polytope::BuildNewFaces(const DynamicArray<Edge>& edges, DynamicArray<MinkowskiSupport>* vertices, const MinkowskiSupport& newVertex) {
	DynamicArray<Face> newFaces = {};
	const auto newVertexIndex = vertices->GetSize();

	for (const auto& edge : edges) {
		const Face nFace = { edge.a, edge.b, newVertexIndex };
		newFaces.Insert(nFace);
	}

	vertices->Insert(newVertex);

	return newFaces;
}

Vector3f Polytope::GetSupport(const Vector3f& normalizedDirection) const {
	Vector3f output = Vector3f::Zero;
	double maxProjection = std::numeric_limits<float>::lowest();

	for (const MinkowskiSupport& vertex : m_vertices) {
		const float iProjection = vertex.point.Dot(normalizedDirection);

		if (iProjection > maxProjection) {
			maxProjection = iProjection;
			output = vertex.point;
		}
	}

	return output;
}
#endif

bool MinkowskiHull::SurpasesMinDepth() const {
	return m_surpassesMinDepth;
}

Vector3f MinkowskiHull::GetMtv() const {
	return m_mtv;
}

Vector3f MinkowskiHull::GetNormalizedMtv() const {
	return m_mtv.GetNormalized();
}

std::array<UIndex64, 3> MinkowskiHull::GetFirstFaceVerticesIds() const {
	const Face& face = m_faces[0];

	const MinkowskiSupport& a = m_vertices[face[0]];
	const MinkowskiSupport& b = m_vertices[face[1]];
	const MinkowskiSupport& c = m_vertices[face[2]];

	return { a.originalVertexIdA, b.originalVertexIdA, c.originalVertexIdA };
}

std::array<UIndex64, 3> MinkowskiHull::GetSecondFaceVerticesIds() const {
	const Face& face = m_faces[0];

	const MinkowskiSupport& a = m_vertices[face[0]];
	const MinkowskiSupport& b = m_vertices[face[1]];
	const MinkowskiSupport& c = m_vertices[face[2]];

	return { a.originalVertexIdB, b.originalVertexIdB, c.originalVertexIdB };
}

MinkowskiHull::Volume MinkowskiHull::GetReferenceId() const {
	return m_referenceVolume;
}
