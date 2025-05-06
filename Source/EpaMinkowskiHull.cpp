#include "EpaMinkowskiHull.h"

#include "Simplex.h"
#include "NumericTypes.h"
#include "Vector3.hpp"
#include "Assert.h"
#include "InvalidObjectStateException.h"

#include "OSKengine.h"
#include "Logger.h"


OSK::Vector3f OSK::COLLISION::EpaMinkowskiHull::GetMtv() const {
	return m_faceNormals[m_minDistanceIndex] * m_distances[m_minDistanceIndex];
}

OSK::Vector3f GetBarycentricCoordinates(const OSK::Vector3f& p, const OSK::Vector3f& a, const OSK::Vector3f& b, const OSK::Vector3f& c) {
	auto output = OSK::Vector3f::Zero;

	auto v0 = b - a, v1 = c - a, v2 = p - a;
	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);
	float denom = d00 * d11 - d01 * d01;
	
	output.y = (d11 * d20 - d01 * d21) / denom;
	output.z = (d00 * d21 - d01 * d20) / denom;
	output.x = 1.0f - output.y - output.z;

	return output;
}

OSK::Vector3f OSK::COLLISION::EpaMinkowskiHull::GetContactPoint() const {
	const auto& face = m_faces[m_minDistanceIndex];
	const auto& normal = m_faceNormals[m_minDistanceIndex];
	const auto distance = m_distances[m_minDistanceIndex];

	const auto& v1 = m_vertices[face[0]];
	const auto& v2 = m_vertices[face[1]];
	const auto& v3 = m_vertices[face[2]];
	
	const auto pointInHull = Vector3f::Zero + normal * distance;

	const auto coords = GetBarycentricCoordinates(pointInHull, v1.point, v2.point, v3.point);

	OSK_ASSERT(
		glm::abs((coords.x + coords.y + coords.z) - 1.0f) < 0.01f, 
		InvalidObjectStateException("Coordenadas baricéntricas inválidas."));

	return v1.worldSpacePointA * coords.x
		 + v2.worldSpacePointA * coords.y
		 + v3.worldSpacePointA * coords.z;
}

bool OSK::COLLISION::EpaMinkowskiHull::SurpasesMinDepth() const {
	return m_surpassesMinDepth;
}

OSK::COLLISION::EpaMinkowskiHull OSK::COLLISION::EpaMinkowskiHull::From(const Simplex& startingSimplex, const IGjkCollider& first, const IGjkCollider& second) {
	auto polytope = EpaMinkowskiHull(startingSimplex);
	polytope.Expand(first, second);

	return polytope;
}

OSK::COLLISION::EpaMinkowskiHull::EpaMinkowskiHull(const Simplex& startingSimplex) {
	const auto& simplexVertices = startingSimplex.GetVertices();

	for (UIndex64 i = 0; i < startingSimplex.GetVerticesCount(); i++) {
		m_vertices.Insert(simplexVertices[i]);

		if (simplexVertices[i].point.Equals(Vector3f::Zero, MinDistanceThreshold)) {
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

	UpdateNormalsAndDistances();
}

void OSK::COLLISION::EpaMinkowskiHull::Expand(const IGjkCollider& first, const IGjkCollider& second) {
	constexpr static USize64 numIterations = 24;

	UIndex64 iteration = 0;
	for (; iteration < numIterations; iteration++) {

		// Actualizamos las normales y las distancias,
		// para asegurarnos de que tenemos un estado correcto.
		UpdateNormalsAndDistances();

		if (!m_surpassesMinDepth) {
			break;
		}

		if (m_earlyExit) {
			break;
		}

		// Distancia mínima actual.
		const auto currentDistance = m_distances[m_minDistanceIndex];

		// Vector normal de la distancia mínima actual.
		const auto currentNormal = m_faceNormals[m_minDistanceIndex];


		// Si la distancia está ya por debajo de un número bajo,
		// lo consideramos válido.
		if (currentDistance < MinDistanceThreshold) {
			m_surpassesMinDepth = false;

			break;
		}

		// Punto del hull más hacia afuera en la dirección
		// actual (normal de la cara con distancia mínima).
		MinkowskiSupport support = GetMinkowskiSupport(first, second, currentNormal);

		// Punto actual más hacia afuera en la dirección actual.
		const auto furthestPoint = this->GetSupport(currentNormal);

		// Distancia del soporte respecto al origen en la dirección actual.
		const float supportProjection = support.point.Dot(currentNormal);

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
			break;
		}

		if (alreadyContainsPoint) {
			break;
		}


		// Aristas que forma el borde de las caras que están
		// mirando hacia el nuevo punto.
		DynamicArray<Edge> uniqueEdges;

		// ELiminamos las caras que estén mirando hacia el nuevo punto.
		// Esto es debido a que, al añadir un nuevo punto, hay que 
		// reconstruir las caras con el nuevo punto.
		for (UIndex64 faceId = 0; faceId < m_faces.GetSize(); faceId++) {
			const auto& face = m_faces[faceId];
			const auto vertexIndex = face[0];

			const Vector3f faceVertex = m_vertices[vertexIndex].point;
			const Vector3f faceNormal = m_faceNormals[faceId];

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
			m_faces[faceId]       = m_faces.Pop();
			m_faceNormals[faceId] = m_faceNormals.Pop();
			m_distances[faceId]   = m_distances.Pop();

			if (faceId == m_minDistanceIndex) {
				m_minDistanceIndex = std::numeric_limits<UIndex64>::max();
			}

			faceId--;
		}

		// Nuevas caras construidas a partir de las aristas
		// y el nuevo vértice.
		m_faces.InsertAll(BuildNewFaces(uniqueEdges, &m_vertices, support));
	}
}

void OSK::COLLISION::EpaMinkowskiHull::AddUniqueEdge(DynamicArray<Edge>* edges, UIndex64 vertexA, UIndex64 vertexB) {
	// Una arista es la misma independientemente del 
	// orden de los vértices.
	const Edge edge = { .a = vertexA, .b = vertexB };

	const UIndex64 edgeIndex = edges->GetIndexOf(edge);

	if (edgeIndex != DynamicArray<EpaMinkowskiHull::Edge>::NotFoundIndex) {
		edges->RemoveAt(edgeIndex);
	}
	else {
		edges->Insert(edge);
	}
}

OSK::DynamicArray<OSK::COLLISION::EpaMinkowskiHull::Face> OSK::COLLISION::EpaMinkowskiHull::BuildNewFaces(const DynamicArray<Edge>& edges, DynamicArray<MinkowskiSupport>* vertices, const MinkowskiSupport& newVertex) {
	DynamicArray<Face> newFaces = {};

	// El vértice se añadirá al final de la lista.
	const auto newVertexIndex = vertices->GetSize();

	// Por cada arista se construye una nueva cara.
	for (const auto& edge : edges) {
		const Face nFace = { edge.a, edge.b, newVertexIndex };
		newFaces.Insert(nFace);
	}

	// Se añade el vértice.
	vertices->Insert(newVertex);

	return newFaces;
}

void OSK::COLLISION::EpaMinkowskiHull::UpdateNormalsAndDistances() {
	// Construir nuevos normales:
	for (UIndex64 i = m_faceNormals.GetSize(); i < m_faces.GetSize(); i++) {
		const auto& face = m_faces[i];

		const Vector3f& a = m_vertices[face[0]].point;
		const Vector3f& b = m_vertices[face[1]].point;
		const Vector3f& c = m_vertices[face[2]].point;

		const Vector3f ab = b - a;
		const Vector3f ac = c - a;

		m_faceNormals.Insert(ab.Cross(ac).GetNormalized());

		if (m_faceNormals[i].IsNaN()) {
			m_earlyExit = true;
			return;
		}

		m_distances.Insert(a.Dot(m_faceNormals[i]));

		OSK_ASSERT(!m_faceNormals[i].IsNaN(), InvalidObjectStateException("Vector NAN"));

		if (m_distances[i] < 0.0f) {
			m_faceNormals[i] *= -1.0f;
			m_distances[i]   *= -1.0f;
		}
	}

	// Obtenemos la distancia mínima.
	float minDistance = std::numeric_limits<float>::max();
	for (UIndex64 i = 0; i < m_distances.GetSize(); i++) {
		if (m_distances[i] < minDistance) {
			minDistance = m_distances[i];
			m_minDistanceIndex = i;
		}
	}
}

OSK::Vector3f OSK::COLLISION::EpaMinkowskiHull::GetSupport(const Vector3f& normalizedDirection) const {
	Vector3f output = Vector3f::Zero;
	auto maxProjection = std::numeric_limits<float>::lowest();

	for (const auto& vertex : m_vertices) {
		const float projection = vertex.point.Dot(normalizedDirection);

		if (projection > maxProjection) {
			maxProjection = projection;
			output = vertex.point;
		}
	}

	return output;
}

bool OSK::COLLISION::EpaMinkowskiHull::Edge::operator==(const Edge& other) const {
	return (a == other.a && b == other.b) || (a == other.b && b == other.a);
}
