// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "ConvexVolume.h"

#include "Transform3D.h"
#include "FaceProjection.h"

#include <optional>
#include "NotImplementedException.h"
#include "OSKengine.h"
#include "Logger.h"

#include "InvalidArgumentException.h"
#include "InvalidObjectStateException.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;


ConvexVolume ConvexVolume::CreateObb(const Vector3f& size, float bottomHeight) {
	ConvexVolume output;

	const float width = size.x * 0.5f;
	const float height = size.y;
	const float length = size.Z * 0.5f;

	const float top = bottomHeight + height;
	const float bottom = bottomHeight;

	// Arriba
	// width, length
	output.AddFace({
		{ -width, top, -length },
		{ +width, top, -length },
		{ +width, top, +length },
		{ -width, top, +length },
		});

	// Abajo
	// width, length
	output.AddFace({
		{ -width, bottom, -length },
		{ -width, bottom, +length },
		{ +width, bottom, +length },
		{ +width, bottom, -length },
		});


	// Izquierda
	// height, length
	output.AddFace({
		{ -width, bottom, -length },
		{ -width, bottom, +length },
		{ -width, top,    +length },
		{ -width, top,    -length },
		});

	// Derecha
	// height, length
	output.AddFace({
		{ +width, bottom, +length },
		{ +width, bottom, -length },
		{ +width, top,    -length },
		{ +width, top,    +length },
		});


	// Delante
	// width, height
	output.AddFace({
		{ -width, bottom, +length },
		{ +width, bottom, +length },
		{ +width, top,    +length },
		{ -width, top,    +length },
		});

	// Detras
	// width, height
	output.AddFace({
		{ +width, top,    -length },
		{ +width, bottom, -length },
		{ -width, bottom, -length },
		{ -width, top,    -length },
		});

	return output;
}

void ConvexVolume::AddFace(const DynamicArray<Vector3f>& points) {
	OSK_ASSERT(points.GetSize() >= 3,
		InvalidArgumentException("El parámetro points contiene menos de 3 entradas."));
	OSK_ASSERT(points.GetSize() < std::numeric_limits<UIndex32>::max(),
		InvalidArgumentException("El parámetro points contiene demasiadas entradas."));
	OSK_ASSERT(m_vertices.GetSize() < std::numeric_limits<UIndex32>::max(),
		InvalidObjectStateException("El collider tiene demasiados vértices."));
	OSK_ASSERT(m_vertices.GetSize() + points.GetSize() < std::numeric_limits<UIndex32>::max(),
		InvalidObjectStateException("El collider tiene demasiados vértices."));

	const auto numOldVertices = static_cast<USize32>(m_vertices.GetSize());
	const auto numNewVertices = static_cast<USize32>(points.GetSize());

#if 0
	// Primero, comprobamos si alguno de los vértices
	// ya existe en la lista de vértices.
	//
	// Lista que mapea cada nuevo vértice con el índice
	// del vértice antiguo equivalente.
	// Es posible que un vértice no tenga equivalente anterior.
	DynamicArray<std::optional<TIndex>> existingIndices = DynamicArray<std::optional<TIndex>>::CreateResizedArray(numNewVertices);

	for (TIndex newIndex = 0; newIndex < numNewVertices; newIndex++) {
		const Vector3f newVertex = points[newIndex];

		for (TIndex oldIndex = 0; oldIndex < numOldVertices; oldIndex++) {
			const Vector3f oldVertex = vertices[oldIndex];

			if (oldVertex.Equals(newVertex, 0.01f))
				existingIndices[newIndex] = oldIndex;
		}
	}

	// Lista con los nuevos vértices (que no estaban presentes anteriormente).
	DynamicArray<Vector3f> newVertices;
	for (TIndex i = 0; i < numNewVertices; i++)
		if (!existingIndices[i].has_value())
			newVertices.Insert(points[i]);
#endif

	FaceIndices indices = FaceIndices::CreateReservedArray(numNewVertices);
	for (UIndex32 i = 0; i < numNewVertices; i++) {
		indices.Insert(numOldVertices + i);
		m_vertices.Insert(points[i]);
	}

	m_faces.Insert(indices);

	RecalculateCenter();
}

void ConvexVolume::OptimizeAxes() {
	if (m_faces.IsEmpty())
		return;

	DynamicArray<FaceIndices> optimizedFaces = {};
	DynamicArray<UIndex32> duplicatedIds = {};

	for (UIndex32 i = 0; i < m_faces.GetSize() - 1; i++) {

		// Por cada eje original, comprobamos si hay otros
		// ejes equivalentes.

		for (UIndex32 j = i + 1; j < m_faces.GetSize(); j++) {

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
	for (UIndex32 i = 0; i < m_faces.GetSize(); i++)
		if (!duplicatedIds.ContainsElement(i))
			optimizedFaces.Insert(m_faces[i]);

	m_faces = optimizedFaces;
}

void ConvexVolume::RecalculateCenter() {
	m_center = Vector3f::Zero;

	for (const Vector3f vertex : m_vertices)
		m_center += vertex;

	m_center /= static_cast<float>(m_vertices.GetSize());
}

ConvexVolume::Axis ConvexVolume::GetLocalSpaceAxis(UIndex32 faceId) const {
	const FaceIndices& faceIndices = m_faces[faceId];

	const Vector3f pointA = m_vertices.At(faceIndices.At(0));
	const Vector3f pointB = m_vertices.At(faceIndices.At(1));
	const Vector3f pointC = m_vertices.At(faceIndices.At(2));

	const Vector3f vec1 = pointB - pointA;
	const Vector3f vec2 = pointC - pointB;

	const Vector3f cross = vec1.Cross(vec2).GetNormalized();
	const Vector3f vecToCenter = m_center - pointA;
	
	return cross * glm::sign(cross.Dot(vecToCenter));
}

ConvexVolume::Axis ConvexVolume::GetWorldSpaceAxis(UIndex32 faceId) const {
	const FaceIndices& faceIndices = m_faces[faceId];

	const Vector3f pointA = m_transformedVertices.At(faceIndices.At(0));
	const Vector3f pointB = m_transformedVertices.At(faceIndices.At(1));
	const Vector3f pointC = m_transformedVertices.At(faceIndices.At(2));

	const Vector3f vec1 = pointB - pointA;
	const Vector3f vec2 = pointC - pointB;

	const Vector3f cross = vec1.Cross(vec2).GetNormalized();
	const Vector3f vecToCenter = m_center - pointA;

	return cross * glm::sign(cross.Dot(vecToCenter));
}

DetailedCollisionInfo ConvexVolume::GetCollisionInfo(const IBottomLevelCollider& other,
	const Transform3D& thisTransform, const Transform3D& otherTransform) const {

	const auto& otherVolume = (const ConvexVolume&)other;

	// Vértices en world-space de cada collider.
	const auto& transformedVerticesA = this->m_transformedVertices;
	const auto& transformedVerticesB = otherVolume.m_transformedVertices;

	// Vector mínimo que se debe aplicar para separar las entidades,
	// de tal manera que dejen de estar en colisión.

	Vector3f currentAxis = 0.0f;
	UIndex32 mtvFaceIndex = 0;


	float minimumOverlap = std::numeric_limits<float>::max();

	bool isAxisA = false;

	// Comprobamos los ejes de este collider.
	for (UIndex32 faceIndex = 0; faceIndex < (m_faces.GetSize() + otherVolume.m_faces.GetSize()); faceIndex++) {
		const bool isFirstEntity = faceIndex < m_faces.GetSize();

		const Axis axis = isFirstEntity
			? this->GetWorldSpaceAxis(faceIndex)
			: otherVolume.GetWorldSpaceAxis(faceIndex - static_cast<USize32>(m_faces.GetSize()));

		const auto projA = FaceProjection(transformedVerticesA, axis);
		const auto projB = FaceProjection(transformedVerticesB, axis);

		// Si no hay overlap, no hay colisión.
		if (!projA.Overlaps(projB))
			return DetailedCollisionInfo::False();


		// Distancia que los dos objetos se solapan en este eje.
		const float overlap = projA.GetOverlap(projB);

		if (glm::abs(overlap) < glm::abs(minimumOverlap)) {
			minimumOverlap = overlap;
			currentAxis = axis;

			mtvFaceIndex = faceIndex;
			isAxisA = isFirstEntity;
		}
	}
	

	
	// A to B
	const Vector3f mtv = currentAxis.GetNormalized() * minimumOverlap;


	// Obtenemos los puntos de contacto.
	// @see https://dyn4j.org/2011/11/contact-points-using-clipping/


	// El collider de referencia es el collider a partir del cual hemos
	// obtenido el MTV.
	const ConvexVolume& primaryCollider = isAxisA
		? *this
		: otherVolume;

	// El collider secundario es el otro collider.
	const ConvexVolume& secondaryCollider = isAxisA
		? otherVolume
		: *this;

	// Los vértices, transfomrados a espacio del mundo,
	// del collider de referencia.
	const DynamicArray<Vector3f>& primaryVertices = isAxisA
		? transformedVerticesA
		: transformedVerticesB;

	// Los vértices, transfomrados a espacio del mundo,
	// del collider incidente.
	const DynamicArray<Vector3f>& secondaryVertices = isAxisA
		? transformedVerticesB
		: transformedVerticesA;


	// MTV normalizado, expresado en la dirección Referencia -> Incidente.
	const Vector3f normalizedDirectedMtv = isAxisA
		?  mtv.GetNormalized()
		: -mtv.GetNormalized();

	
	// Debemos obtener el punto del collider incidente que está
	// más hacia el collider de referencia.
	UIndex32 furthestIncidentPointIndex = 0;
	float furthestIncidentPointProjection = std::numeric_limits<float>::max();

	for (UIndex32 i = 0; i < secondaryVertices.GetSize(); i++) {
		const auto& vertex = secondaryVertices[i];
		
		const float dot = vertex.Dot(normalizedDirectedMtv);
		if (dot < furthestIncidentPointProjection) {
			furthestIncidentPointProjection = dot;
			furthestIncidentPointIndex = i;
		}
	}


	// Índices de las caras del segundo collider que contienen
	// el vértice más hacia la referencia.
	const DynamicArray<UIndex32> secondaryFacesIndices 
		= secondaryCollider.GetFaceIndicesWithVertex(secondaryCollider.m_vertices[furthestIncidentPointIndex]);


	// Obtenemos la cara del collider incidente que está más paralela a
	// la cara del MTV.
	UIndex32 mostParallelFaceIndex = 0;
	Vector3f secondFaceNormal = Vector3f::Zero;

	{
		// Dot de dos vectores paralelos es 1.
		float mostParallelDot = std::numeric_limits<float>::max();

		for (const UIndex32 faceId : secondaryFacesIndices) {
			const Vector3f faceNormal = secondaryCollider.GetWorldSpaceAxis(faceId);
			const float dot = glm::abs(faceNormal.Dot(normalizedDirectedMtv) - 1.0f);
			
			if (dot < mostParallelDot) {
				mostParallelFaceIndex = faceId;
				mostParallelDot = dot;
				secondFaceNormal = faceNormal;
			}
		}
	}

	// Índice de la cara incidente.
	const UIndex32 incidentFaceIndex = mostParallelFaceIndex;

	// Índice de la cara del collider de referencia a partir
	// del que hemos sacado el MTV.
	const UIndex64 referenceFaceIndex = isAxisA
		? mtvFaceIndex
		: mtvFaceIndex - m_faces.GetSize();

	// Puntos de contacto.
	// Se inicializan a los puntos de la cara incidente, y después
	// se van ajustando para representar el área de colisión.
	DynamicArray<Vector3f> contactPoints = DynamicArray<Vector3f>::CreateReservedArray(secondaryCollider.m_faces[incidentFaceIndex].GetSize());
	for (const UIndex32 index : secondaryCollider.m_faces[incidentFaceIndex])
		contactPoints.Insert(secondaryVertices[index]);


	// --------------------------- //

	// Cara de referencia.
	const FaceIndices& referenceFace = primaryCollider.m_faces[referenceFaceIndex];

	// Número de vértices en la cara de referencia.
	const auto numReferenceVertices = referenceFace.GetSize();
	
	// Obtenemos los pares de vértices consecutivos de la cara de referencia.
	for (UIndex64 v = 0; v < numReferenceVertices; v++) {
		const UIndex64 firstReferenceVertexIndex  = referenceFace[(v + 0) % numReferenceVertices];
		const UIndex64 secondReferenceVertexIndex = referenceFace[(v + 1) % numReferenceVertices];
		const UIndex64 thirdReferenceVertexIndex  = referenceFace[(v + 2) % numReferenceVertices];

		// Definimos un plano a partir de una línea (definida por dos vértices consecutivos)
		// en dirección al centro.
		const Vector3f referenceVertex1 = primaryVertices[firstReferenceVertexIndex];
		const Vector3f referenceVertex2 = primaryVertices[secondReferenceVertexIndex];
		const Vector3f referenceVertex3 = primaryVertices[thirdReferenceVertexIndex];

		const Vector3f lineDirection = (referenceVertex2 - referenceVertex1).Cross(referenceVertex3 - referenceVertex2).GetNormalized();

		// Dirección a la que deben estár los puntos.
		const Vector3f direction = normalizedDirectedMtv.Cross(lineDirection).GetNormalized();

		for (UIndex64 j = 0; j < contactPoints.GetSize(); j++) {
			Vector3f& point = contactPoints[j];

			const Vector3f relativePosition = point - referenceVertex1;

			const float dot = relativePosition.Dot(direction);

			if (dot < 0.0f)
				point += -direction * dot;
		}
	}


	DynamicArray<Vector3f> finalContactPoints = DynamicArray<Vector3f>::CreateReservedArray(contactPoints.GetSize());
	float currentFurthestDistance = std::numeric_limits<float>::lowest();

	for (const Vector3f& point : contactPoints) {
		// Mayor distanca = más metido.
		const float distance = point.Dot(-normalizedDirectedMtv);

		if (distance > currentFurthestDistance)
			currentFurthestDistance = distance;
	}

	for (const Vector3f& point : contactPoints) {
		// Mayor distanca = más metido.
		const float distance = point.Dot(-normalizedDirectedMtv);

		if (glm::abs(distance - currentFurthestDistance) < 0.01f)
			finalContactPoints.Insert(point);
	}

	return DetailedCollisionInfo::True(
		mtv,
		finalContactPoints,
		mtv,
		secondFaceNormal,
		!isAxisA
	);
}

DynamicArray<UIndex32> ConvexVolume::GetFaceIndicesWithVertex(const Vector3f& vertex) const {
	DynamicArray<UIndex32> output = DynamicArray<UIndex32>::CreateReservedArray(m_faces.GetSize());

	for (UIndex32 i = 0; i < m_faces.GetSize(); i++) {
		const auto& face = m_faces[i];

		bool containstVertex = false;
		for (UIndex32 v = 0; v < face.GetSize(); v++) {
			const Vector3f thisVertex = m_vertices[face[v]];
			if (thisVertex.Equals(vertex, 0.001f)) {
				containstVertex = true;
				break;
			}
		}

		if (containstVertex)
			output.Insert(i);
	}

	return output;
}

bool ConvexVolume::ContainsPoint(const Vector3f& point) const {
	OSK_ASSERT(false, NotImplementedException());
	return false;
}

Vector3f ConvexVolume::GetFurthestPoint(Vector3f direction) const {
	Vector3f output = Vector3f::Zero;
	float outputDistance = std::numeric_limits<float>::lowest();

	for (const Vector3f vertex : m_vertices) {
		const float distance = direction.Dot(vertex);

		if (distance > outputDistance) {
			outputDistance = distance;
			output = vertex;
		}
	}

	return output;
}

void ConvexVolume::Transform(const Transform3D& transform) {
	m_transformedVertices.Empty();
	m_transformedCenter = Vector3f::Zero;

	for (const Vector3f vertex : m_vertices) {
		m_transformedVertices.Insert(transform.TransformPoint(vertex));
		m_transformedCenter += vertex;
	}

	m_transformedCenter /= static_cast<float>(m_transformedVertices.GetSize());
}

const DynamicArray<Vector3f>& ConvexVolume::GetLocalSpaceVertices() const {
	return m_vertices;
}

DynamicArray<ConvexVolume::FaceIndices> ConvexVolume::GetFaceIndices() const {
	return m_faces;
}
