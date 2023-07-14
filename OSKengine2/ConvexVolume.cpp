// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "ConvexVolume.h"

#include "Transform3D.h"
#include "FaceProjection.h"

#include <optional>
#include "NotImplementedException.h"
#include "OSKengine.h"
#include "Logger.h"

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
	output.AddFace({
		{ -width, top, -length },
		{ +width, top, -length },
		{ +width, top, +length },
		{ -width, top, +length },
		});

	// Abajo
	output.AddFace({
		{ -width, bottom, -length },
		{ -width, bottom, +length },
		{ +width, bottom, +length },
		{ +width, bottom, -length },
		});


	// Izquierda
	output.AddFace({
		{ -width, bottom, -length },
		{ -width, bottom, +length },
		{ -width, top,    +length },
		{ -width, top,    -length },
		});

	// Derecha
	output.AddFace({
		{ +width, bottom, +length },
		{ +width, bottom, -length },
		{ +width, top,    -length },
		{ +width, top,    +length },
		});


	// Delante
	output.AddFace({
		{ -width, bottom, +length },
		{ +width, bottom, +length },
		{ +width, top,    +length },
		{ -width, top,    +length },
		});

	// Detras
	output.AddFace({
		{ +width, top,    -length },
		{ +width, bottom, -length },
		{ -width, bottom, -length },
		{ -width, top,    -length },
		});

	return output;
}

void ConvexVolume::AddFace(const DynamicArray<Vector3f>& points) {
	const USize64 numOldVertices = vertices.GetSize();
	const USize64 numNewVertices = points.GetSize();

	/* 
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
	*/

	FaceIndices indices = FaceIndices::CreateReservedArray(numNewVertices);
	for (UIndex64 i = 0; i < numNewVertices; i++) {
		indices.Insert(numOldVertices + i);
		vertices.Insert(points[i]);
	}

	faces.Insert(indices);
}

void ConvexVolume::OptimizeAxes() {
	if (faces.IsEmpty())
		return;

	DynamicArray<FaceIndices> optimizedFaces = {};
	DynamicArray<UIndex32> duplicatedIds = {};

	for (UIndex32 i = 0; i < faces.GetSize() - 1; i++) {

		// Por cada eje original, comprobamos si hay otros
		// ejes equivalentes.

		for (UIndex32 j = i + 1; j < faces.GetSize(); j++) {

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
	for (UIndex32 i = 0; i < faces.GetSize(); i++)
		if (!duplicatedIds.ContainsElement(i))
			optimizedFaces.Insert(faces[i]);

	faces = optimizedFaces;
}

ConvexVolume::Axis ConvexVolume::GetLocalSpaceAxis(UIndex32 faceId) const {
	const FaceIndices& faceIndices = faces[faceId];

	const Vector3f pointA = vertices.At(faceIndices.At(0));
	const Vector3f pointB = vertices.At(faceIndices.At(1));
	const Vector3f pointC = vertices.At(faceIndices.At(2));

	const Vector3f vec1 = pointB - pointA;
	const Vector3f vec2 = pointC - pointA;

	return vec1.Cross(vec2).GetNormalized();
}

ConvexVolume::Axis ConvexVolume::GetWorldSpaceAxis(UIndex32 faceId, const Transform3D& transform) const {
	const FaceIndices& faceIndices = faces[faceId];

	const Vector3f pointA = vertices[faceIndices[0]];
	const Vector3f pointB = vertices[faceIndices[1]];
	const Vector3f pointC = vertices[faceIndices[2]];

	const Vector3f transformedA = transform.TransformPoint(pointA);
	const Vector3f transformedB = transform.TransformPoint(pointB);
	const Vector3f transformedC = transform.TransformPoint(pointC);

	const Vector3f vec1 = transformedB - transformedA;
	const Vector3f vec2 = transformedC - transformedB;

	return vec1.Cross(vec2).GetNormalized();
}

DetailedCollisionInfo ConvexVolume::GetCollisionInfo(const IBottomLevelCollider& other,
	const Transform3D& thisTransform, const Transform3D& otherTransform) const {

	const auto& otherVolume = (const ConvexVolume&)other;

	// Vértices en world-space de cada collider.
	const auto transformedVerticesA = this->GetWorldSpaceVertices(thisTransform);
	const auto transformedVerticesB = otherVolume.GetWorldSpaceVertices(otherTransform);

	// Vector mínimo que se debe aplicar para separar las entidades,
	// de tal manera que dejen de estar en colisión.

	Vector3f currentAxis = 0.0f;
	UIndex64 mtvFaceIndex = 0;

	float minimumOverlap = std::numeric_limits<float>::max();

	bool isAxisA = false;

	// Comprobamos los ejes de este collider.
	for (UIndex64 faceIndex = 0; faceIndex < (faces.GetSize() + otherVolume.faces.GetSize()); faceIndex++) {
		const bool isFirstEntity = faceIndex < faces.GetSize();

		const Axis axis = isFirstEntity
			? this->GetWorldSpaceAxis(faceIndex, thisTransform)
			: otherVolume.GetWorldSpaceAxis(faceIndex - faces.GetSize(), otherTransform);

		const auto projA = FaceProjection(transformedVerticesA, axis);
		const auto projB = FaceProjection(transformedVerticesB, axis);

		// Si no hay overlap, no hay colisión.
		if (!projA.Overlaps(projB))
			return DetailedCollisionInfo::False();


		// Distancia que los dos objetos se solapan en este eje.
		const float overlap = isAxisA 
			? projA.GetOverlap(projB)
			: projB.GetOverlap(projA);

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


	// El collider primario es el collider a partir del cual hemos
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
	const Vector3f normalizedDirectedMtv = mtv.GetNormalized();


	// Transform del collider incidente.
	const Transform3D& secondaryTransform = otherTransform;

	
	// Debemos obtener el punto del collider incidente que está
	// más hacia el collider de referencia.
	UIndex32 furthestIncidentPointIndex = 0;
	float furthestIncidentPointProjection = std::numeric_limits<float>::lowest();

	for (UIndex32 i = 0; i < secondaryVertices.GetSize(); i++) {
		const auto& vertex = secondaryVertices[i];
		
		const float dot = vertex.Dot(normalizedDirectedMtv);
		if (dot > furthestIncidentPointProjection) {
			furthestIncidentPointProjection = dot;
			furthestIncidentPointIndex = i;
		}
	}


	// Índices de las caras del segundo collider que contienen
	// el vértice más alejado.
	const DynamicArray<UIndex32> secondaryFacesIndices = secondaryCollider.GetFaceIndicesWithVertex(secondaryCollider.vertices[furthestIncidentPointIndex]);


	// Obtenemos la cara del collider incidente que está más paralela a
	// la cara del MTV.
	UIndex32 mostParallelFaceIndex = 0;

	{
		float mostParallelDot = std::numeric_limits<float>::lowest();
		for (const UIndex32 faceId : secondaryFacesIndices) {
			const Vector3f faceNormal = secondaryCollider.GetWorldSpaceAxis(faceId, secondaryTransform);
			const float dot = glm::abs(faceNormal.Dot(normalizedDirectedMtv));

			if (dot > mostParallelDot) {
				mostParallelFaceIndex = faceId;
				mostParallelDot = dot;
			}
		}
	}

	// Índice de la cara incidente.
	const UIndex32 incidentFaceIndex = mostParallelFaceIndex;

	// Índice de la cara del collider de referencia a partir
	// del que hemos sacado el MTV.
	const UIndex64 referenceFaceIndex = isAxisA
		? mtvFaceIndex
		: mtvFaceIndex - faces.GetSize();

	// Puntos de contacto.
	// Se inicializan a los puntos de la cara incidente, y después
	// se van ajustando para representar el área de colisión.
	DynamicArray<Vector3f> contactPoints = DynamicArray<Vector3f>::CreateReservedArray(secondaryCollider.faces[incidentFaceIndex].GetSize());
	for (const UIndex32 index : secondaryCollider.faces[incidentFaceIndex])
		contactPoints.Insert(secondaryVertices[index]);


	// --------------------------- //

	// Cara de referencia.
	const FaceIndices& referenceFace = primaryCollider.faces[referenceFaceIndex];

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
		const Vector3f direction = -normalizedDirectedMtv.Cross(lineDirection).GetNormalized();

		for (UIndex64 j = 0; j < contactPoints.GetSize(); j++) {
			Vector3f& point = contactPoints[j];

			const Vector3f relativePosition = point - referenceVertex1;

			const float dot = relativePosition.Dot(direction);

			if (dot < 0.0f)
				point += -direction * dot;
		}
	}


	DynamicArray<Vector3f> finalContactPoints = DynamicArray<Vector3f>::CreateReservedArray(contactPoints.GetSize());
	float currentFurthestDistance = -99999.9f;
	for (const Vector3f& point : contactPoints) {
		// Mayor distanca = más metido.
		const float distance = point.Dot(normalizedDirectedMtv);

		if (glm::abs(distance - currentFurthestDistance) < 0.001f) {
			finalContactPoints.Insert(point);
		}
		else if (distance > currentFurthestDistance) {
			currentFurthestDistance = distance;
		
			finalContactPoints.Empty();
			finalContactPoints.Insert(point);
		}
	}

	// for (auto& point : finalContactPoints)
	//	point += directedMtv * 0.5f;

// #define PRIMARY_POINTS
// #define SECONDARY_POINTS
#ifdef PRIMARY_POINTS
	finalContactPoints.Empty();
	for (TIndex i = 0; i < referenceFace.GetSize(); i++)
		finalContactPoints.Insert(primaryVertices[referenceFace[i]]);
#elif defined(SECONDARY_POINTS)
	finalContactPoints.Empty();
	for (TIndex i = 0; i < secondaryCollider.faces[incidentFaceIndex].GetSize(); i++)
		finalContactPoints.Insert(secondaryVertices[secondaryCollider.faces[incidentFaceIndex][i]]);
#endif

	return DetailedCollisionInfo::True(mtv, finalContactPoints, false);
}

DynamicArray<UIndex32> ConvexVolume::GetFaceIndicesWithVertex(const Vector3f& vertex) const {
	DynamicArray<UIndex32> output = DynamicArray<UIndex32>::CreateReservedArray(faces.GetSize());

	for (UIndex32 i = 0; i < faces.GetSize(); i++) {
		const auto& face = faces[i];

		bool containstVertex = false;
		for (UIndex32 v = 0; v < face.GetSize(); v++) {
			const Vector3f thisVertex = vertices[face[v]];
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

	for (const auto& vertex : vertices) {
		const float distance = direction.Dot(vertex);

		if (distance > outputDistance) {
			outputDistance = distance;
			output = vertex;
		}
	}

	return output;
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
