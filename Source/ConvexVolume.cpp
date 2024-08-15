#include "ConvexVolume.h"

#include "Transform3D.h"
#include "FaceProjection.h"

#include <optional>
#include <set>
#include <unordered_set>
#include "NotImplementedException.h"
#include "OSKengine.h"
#include "Logger.h"

#include "Simplex.h"
#include "MinkowskiHull.h"
#include "Clipping.h"

#include "Math.h"

#include "InvalidArgumentException.h"
#include "InvalidObjectStateException.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;
using namespace OSK::PERSISTENCE;


OwnedPtr<IBottomLevelCollider> ConvexVolume::CreateCopy() const {
	return new ConvexVolume(*this);
}

ConvexVolume ConvexVolume::CreateObb(const Vector3f& size) {
	ConvexVolume output;

	const float width = size.x * 2.0f;
	const float height = size.y * 2.0f;
	const float length = size.z * 2.0f;

	const float top = height * 0.5f;
	const float bottom = -height * 0.5f;
	const float front = length * 0.5f;
	const float back = -length * 0.5f;
	const float left = -width * 0.5f;
	const float right = width * 0.5f;

	// Arriba
	// width, length
	output.AddFace({
		{ left, top, back },
		{ right, top, back },
		{ right, top, front },
		{ left, top, front },
		});

	// Abajo
	// width, length
	output.AddFace({
		{ left, bottom, back },
		{ left, bottom, front },
		{ right, bottom, front },
		{ right, bottom, back },
		});


	// Izquierda
	// height, length
	output.AddFace({
		{ left, bottom, back },
		{ left, bottom, front },
		{ left, top,    front },
		{ left, top,    back },
		});

	// Derecha
	// height, length
	output.AddFace({
		{ right, bottom, front },
		{ right, bottom, back },
		{ right, top,    back },
		{ right, top,    front },
		});


	// Delante
	// width, height
	output.AddFace({
		{ left, bottom, front },
		{ right, bottom, front },
		{ right, top,    front },
		{ left, top,    front },
		});

	// Detras
	// width, height
	output.AddFace({
		{ right, top,    back },
		{ right, bottom, back },
		{ left, bottom, back },
		{ left, top,    back },
		});

	output.MergeFaces();

	return output;
}

void ConvexVolume::AddOffset(const Vector3f& offset) {
	for (auto& v : m_vertices) {
		v += offset;
	}
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

	FaceIndices indices = FaceIndices::CreateReserved(numNewVertices);
	for (UIndex32 i = 0; i < numNewVertices; i++) {
		indices.Insert(numOldVertices + i);
		m_vertices.Insert(points[i]);
		m_transformedVertices.Insert(points[i]);
	}

	m_faces.Insert(indices);
	m_axes.Insert(GetWorldSpaceAxis(m_faces.GetSize() - 1));

	RecalculateCenter();
}

bool ConvexVolume::AreFacesEquivalent(UIndex32 firstFace, UIndex32 secondFace) const {
	const Axis axis1 = GetLocalSpaceAxis(firstFace);
	const Axis axis2 = GetLocalSpaceAxis(secondFace);

	const bool isSameAxis = axis1 == axis2 || axis1 == -axis2;

	bool hasCommonPoints = false;

	for (const auto& indexA : m_faces[firstFace]) {
		for (const auto& indexB : m_faces[secondFace]) {
			if (indexA == indexB) {
				hasCommonPoints = true;
				break;
			}
		}

		if (hasCommonPoints) {
			break;
		}
	}

	return isSameAxis && hasCommonPoints;
}

void ConvexVolume::MergeVertices() {
	if (m_vertices.IsEmpty())
		return;
	
	// Array con los vértices finales (sin duplicados).
	DynamicArray<Vector3f> nVertices = DynamicArray<Vector3f>::CreateReserved(m_vertices.GetSize());
	
	// Old vertex ID -> new vertex ID.
	// Para poder actualizar los índices de
	// las caras.
	std::unordered_map<UIndex32, UIndex32> oldToNewId;

	for (UIndex32 originalId = 0; originalId < m_vertices.GetSize(); originalId++) {
		const Vector3f originalVertex = m_vertices[originalId];

		// Sólamente añadimos el vértice al array si no estaba previamente.
		// Ningún vértice se añade más de una vez.
		bool containsVertex = false;
		for (const auto& v : nVertices) {
			if (v.Equals(originalVertex, 0.00001f)) {
				containsVertex = true;
				break;
			}
		}

		if (!containsVertex) {
			nVertices.Insert(originalVertex);
		}

		// Índice del vértice en el nuevo array.
		UIndex32 newId = 0;
		for (; newId < nVertices.GetSize(); newId++) {
			if (nVertices[newId].Equals(originalVertex, 0.00001f)) {
				break;
			}
		}

		oldToNewId[originalId] = newId;
	}
	
	// Actualización de las caras.
	for (auto& face : m_faces) {
		for (auto& index : face) {
			index = oldToNewId[index];
		}
	}

	m_vertices = nVertices;
	m_transformedVertices = nVertices;

	RecalculateCenter();
}

void ConvexVolume::NormalizeFaces() {
	for (UIndex32 i = 0; i < m_faces.GetSize(); i++) {
		NormalizeFace(i);
	}
}

void ConvexVolume::NormalizeFace(UIndex32 faceIndex) {
	
	// Cara procesada
	FaceIndices& face = m_faces[faceIndex];

	// Vector normal de la cara.
	const Vector3f faceNormal = GetLocalSpaceAxis(faceIndex);


	// Nueva lista con los índices de los vértices.
	DynamicArray<UIndex32> newIndices = {};


	// Centro de la cara.
	Vector3f center = Vector3f::Zero;

	// Vértices que faltan por procesar.
	std::set<UIndex32> remainingVertices = {};


	// Calculamos centro y vértices por procesar.
	for (UIndex32 i : face) {
		center += m_vertices[i];
		remainingVertices.insert(i);
	}

	center /= static_cast<float>(m_vertices.GetSize());


	// ID del vértice que está siendo procesado.
	UIndex32 currentVertexId = *remainingVertices.begin();

	remainingVertices.erase(currentVertexId);

	newIndices.Insert(currentVertexId);

	Vector3f currentVertex = m_vertices[currentVertexId];

	while (!remainingVertices.empty()) {
		float minAngle = std::numeric_limits<float>::max();
		UIndex32 nextVertexId = 0;

		for (const UIndex32 i : remainingVertices) {
			const Vector3f newVertex = m_vertices[i];

			const Vector3f refToCenter = center - currentVertex;
			const Vector3f newToCenter = center - newVertex;

			// Get direction
			const Vector3f direction = refToCenter.Cross(faceNormal).GetNormalized();
			const float newDot = newToCenter.Dot(direction);

			float angleDeg = refToCenter.GetAngle(newToCenter);
			
			if (newDot < 0.0f)
				angleDeg = (180.0f - angleDeg) + 180.0f;

			if (angleDeg < minAngle) {
				minAngle = angleDeg;
				nextVertexId = i;
			}
		}

		remainingVertices.erase(nextVertexId);
		newIndices.Insert(nextVertexId);

		currentVertex = m_vertices[nextVertexId];
	}

	face = newIndices;
}

void ConvexVolume::MergeFaces() {
	if (m_faces.IsEmpty())
		return;

	MergeVertices();

	// ID de las caras que deben eliminarse y que serán
	// reemplazadas.
	std::unordered_set<UIndex32> facesToDelete = {};

	// Contiene los índices de cada nueva cara.
	// Índices de caras.
	DynamicArray<std::unordered_set<UIndex32>> newFaces = {};


	for (UIndex32 faceA = 0; faceA < m_faces.GetSize() - 1; faceA++) {
		if (facesToDelete.contains(faceA))
			continue;

		// IDs de las caras equivalentes a faceA.
		std::unordered_set<UIndex32> equivalentFaces{};

		// Por cada eje original, comprobamos si hay otros
		// ejes equivalentes.
		for (UIndex32 faceB = faceA + 1; faceB < m_faces.GetSize(); faceB++) {
			if (facesToDelete.contains(faceB))
				continue;

			// Si el otro eje es igual, lo marcamos para no añadirlo
			// a la lista optimizada.
			if (AreFacesEquivalent(faceA, faceB)) {
				equivalentFaces.insert(faceA);
				equivalentFaces.insert(faceB);

				facesToDelete.insert(faceA);
				facesToDelete.insert(faceB);
			}
		}

		if (!equivalentFaces.empty()) {
			newFaces.Insert(equivalentFaces);
		}
	}

	DynamicArray<FaceIndices> optimizedFaces{};

	// Añadimos las caras que no han sido eliminadas.
	for (UIndex32 i = 0; i < m_faces.GetSize(); i++) {
		if (!facesToDelete.contains(i)) {
			optimizedFaces.Insert(m_faces[i]);
		}
	}

	// Por cada nueva cara...
	for (const auto& nFace : newFaces) {
		FaceIndices newFaceIndices = {};

		// ...obtenemos todos sus índices.
		for (const auto& faceIndex : nFace) {
			newFaceIndices.InsertAll(m_faces[faceIndex]);
		}

		optimizedFaces.Insert(newFaceIndices);
	}

	m_faces = optimizedFaces;

	m_axes.Empty();

	for (UIndex64 i = 0; i < m_faces.GetSize(); i++) {
		m_axes.Insert(GetWorldSpaceAxis(i));
	}

	NormalizeFaces();
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

	const float centerProjection = m_center.Dot(cross);
	const float vecProjection = pointA.Dot(cross);

	return cross * MATH::Sign(vecProjection - centerProjection);
}

ConvexVolume::Axis ConvexVolume::GetWorldSpaceAxis(UIndex32 faceId) const {
	const FaceIndices& faceIndices = m_faces[faceId];

	const Vector3f pointA = m_transformedVertices.At(faceIndices.At(0));
	const Vector3f pointB = m_transformedVertices.At(faceIndices.At(1));
	const Vector3f pointC = m_transformedVertices.At(faceIndices.At(2));

	const Vector3f vec1 = pointB - pointA;
	const Vector3f vec2 = pointC - pointB;

	const Vector3f normal = vec1.Cross(vec2).GetNormalized();

	const float centerProjection = m_transformedCenter.Dot(normal);
	const float vecProjection = pointA.Dot(normal);

	return normal * MATH::Sign(vecProjection - centerProjection);
}


UIndex32 ConvexVolume::GetMostParallelFaceIndex(const Vector3f& faceNormal, const DynamicArray<UIndex32>& candidates) const {
	// Dot de dos vectores paralelos es 1.
	float mostParallelDot = std::numeric_limits<float>::max();
	UIndex32 output = 0;

	for (UIndex32 faceId : candidates) {

		const Vector3f nFaceNormal = GetWorldSpaceAxis(faceId);
		const float dot = glm::abs(faceNormal.Dot(nFaceNormal));
		const float diff = glm::abs(1.0f - dot);

		if (diff < mostParallelDot) {
			output = faceId;
			mostParallelDot = dot;
		}
	}

	return output;
}

UIndex32 ConvexVolume::GetMostParallelFaceIndex(const Vector3f& faceNormal) const {
	// Dot de dos vectores paralelos es 1.
	float mostParallelDot = std::numeric_limits<float>::max();
	UIndex32 output = 0;

	for (UIndex32 faceId = 0; faceId < m_faces.GetSize(); faceId++) {

		const Vector3f nFaceNormal = m_axes[faceId];
		const float dot = nFaceNormal.Dot(faceNormal);
		const float diff = glm::abs(1.0f - dot);

		if (diff < mostParallelDot) {
			output = faceId;
			mostParallelDot = dot;
		}
	}

	return output;
}

DetailedCollisionInfo ConvexVolume::GetCollisionInfo(const IBottomLevelCollider& other,
	const Transform3D& thisTransform, const Transform3D& otherTransform) const {

	const auto& otherVolume = (const ConvexVolume&)other;

#define GJK_POINTS
#ifdef GJK_POINTS

	const Simplex simplex = Simplex::GenerateFrom(*this, otherVolume);

	if (!simplex.ContainsOrigin()) {
		return DetailedCollisionInfo::False();
	}

	const auto minkowskiHull = MinkowskiHull::BuildFromVolumes(*this, otherVolume);

	if (!minkowskiHull.SurpasesMinDepth()) {
		return DetailedCollisionInfo::False();
	}

	Vector3f mtv = minkowskiHull.GetMtv();

	const UIndex64 firstFaceId = GetFaceWithVertexIndices(minkowskiHull.GetFirstFaceVerticesIds(), minkowskiHull.GetMtv());
	const UIndex64 secondFaceId = otherVolume.GetFaceWithVertexIndices(minkowskiHull.GetSecondFaceVerticesIds(), -minkowskiHull.GetMtv());

#ifdef OSK_COLLISION_DEBUG
	Engine::GetLogger()->DebugLog(std::format("First vertices count: {}", m_transformedVertices.GetSize()));
	Engine::GetLogger()->DebugLog(std::format("Second vertices count: {}", otherVolume.m_transformedVertices.GetSize()));

	Engine::GetLogger()->DebugLog(std::format("MTV: {:.3f} {:.3f} {:.3f}", mtv.x, mtv.y, mtv.z));
	Engine::GetLogger()->DebugLog(std::format("\tOriginal mtv depth: {:.3f}", expandingPolytope.GetMtv().GetLenght()));
#endif

	const FaceIndices& faceA = m_faces[firstFaceId];
	const FaceIndices& faceB = otherVolume.m_faces[secondFaceId];

	DynamicArray<Vector3f> verticesA = DynamicArray<Vector3f>::CreateReserved(faceA.GetSize());
	for (const auto index : faceA)
		verticesA.Insert(m_transformedVertices[index]);

	DynamicArray<Vector3f> verticesB = DynamicArray<Vector3f>::CreateReserved(faceB.GetSize());
	for (const auto index : faceB)
		verticesB.Insert(otherVolume.m_transformedVertices[index]);

	// Get most incident face:
	MinkowskiHull::Volume incident = MinkowskiHull::Volume::A;
	{
		const float firstDot = m_axes[firstFaceId].Dot(-mtv);
		const float secondDot = otherVolume.m_axes[secondFaceId].Dot(mtv);

		incident = firstDot < secondDot ? MinkowskiHull::Volume::A : MinkowskiHull::Volume::B;
	}

	const Vector3f normal = minkowskiHull.GetNormalizedMtv();
	const Vector3f otherNormal = otherVolume.GetWorldSpaceAxis(secondFaceId);

#ifdef OSK_COLLISION_DEBUG
	Engine::GetLogger()->DebugLog(std::format("First normal: {:.3f} {:.3f} {:.3f}", normal.x, normal.y, normal.z));
	Engine::GetLogger()->DebugLog(std::format("Second normal: {:.3f} {:.3f} {:.3f}", otherNormal.x, otherNormal.y, otherNormal.z));
#endif

	const DynamicArray<Vector3f> finalContactPoints = incident == MinkowskiHull::Volume::A
		? ClipFaces(verticesA, verticesB, normal, otherNormal)
		: ClipFaces(verticesB, verticesA, otherNormal, normal);
	

	return DetailedCollisionInfo::True(
		minkowskiHull.GetMtv(),
		finalContactPoints,
		minkowskiHull.GetNormalizedMtv(),
		otherNormal,
		DetailedCollisionInfo::MtvDirection::A_TO_B
	);

#else
	const auto mtvInfo = SatCollision(*this, other);

	if (!mtvInfo.overlaps) {
		return DetailedCollisionInfo::False();
	}

	const ConvexVolume& primaryCollider = mtvInfo.face.collider == SatInfo::Face::Collider::A
		? *this
		: otherVolume;

	const ConvexVolume& secondaryCollider = mtvInfo.face.collider == SatInfo::Face::Collider::A
		? otherVolume
		: *this;


	// MTV normalizado, expresado en la dirección Referencia -> Incidente.
	const Vector3f normalizedDirectedMtv = mtvInfo.mtv.GetNormalized();


	// Debemos obtener el punto del collider incidente que está
	// más hacia el collider de referencia.
	const UIndex32 furthestIncidentPointIndex = static_cast<UIndex32>(secondaryCollider.GetSupport(normalizedDirectedMtv).originalVertexId);


	// Índices de las caras del segundo collider que contienen
	// el vértice más hacia la referencia.
	const DynamicArray<UIndex32> secondaryFacesIndices
		= secondaryCollider.GetFaceIndicesWithVertex(furthestIncidentPointIndex);


	// Obtenemos la cara del collider incidente que está más paralela a
	// la cara del MTV.
	const UIndex32 incidentFaceIndex = secondaryCollider.GetMostParallelFaceIndex(normalizedDirectedMtv, secondaryFacesIndices);
	const Vector3f secondFaceNormal = secondaryCollider.GetWorldSpaceAxis(incidentFaceIndex);

	DynamicArray<Vector3f> referencePoints = DynamicArray<Vector3f>::CreateReservedArray(primaryCollider.m_faces[mtvInfo.face.index].GetSize());
	for (const UIndex32 index : primaryCollider.m_faces[mtvInfo.face.index])
		referencePoints.Insert(primaryCollider.m_transformedVertices[index]);

	DynamicArray<Vector3f> secondaryPoints = DynamicArray<Vector3f>::CreateReservedArray(secondaryCollider.m_faces[incidentFaceIndex].GetSize());
	for (const UIndex32 index : secondaryCollider.m_faces[incidentFaceIndex])
		referencePoints.Insert(secondaryCollider.m_transformedVertices[index]);

	const DynamicArray<Vector3f> finalContactPoints = ClipFaces(referencePoints, secondaryPoints, normalizedDirectedMtv, secondFaceNormal);

	const Vector3f finalMtv = mtvInfo.mtv * (mtvInfo.face.collider == SatInfo::Face::Collider::A ? 1.0f : -1.0f);
	const Vector3f aToB = mtvInfo.face.collider == SatInfo::Face::Collider::A ? mtvInfo.mtv : secondFaceNormal;
	const Vector3f bToA = mtvInfo.face.collider == SatInfo::Face::Collider::A ? secondFaceNormal : mtvInfo.mtv;

	return DetailedCollisionInfo::True(
		finalMtv,
		finalContactPoints,
		aToB,
		bToA,
		DetailedCollisionInfo::MtvDirection::A_TO_B);

#endif
}

DynamicArray<UIndex32> ConvexVolume::GetFaceIndicesWithVertex(UIndex32 vertexIndex) const {
	DynamicArray<UIndex32> output = DynamicArray<UIndex32>::CreateReserved(m_faces.GetSize());

	for (UIndex32 i = 0; i < m_faces.GetSize(); i++) {
		const auto& face = m_faces[i];

		const bool containstVertex = face.ContainsElement(vertexIndex);

		if (containstVertex)
			output.Insert(i);
	}

	return output;
}

bool ConvexVolume::ContainsPoint(const Vector3f& point) const {
	OSK_ASSERT(false, NotImplementedException());
	return false;
}

GjkSupport ConvexVolume::GetSupport(const Vector3f& direction) const {
	GjkSupport output{};

	double outputDistance = std::numeric_limits<double>::lowest();

	for (UIndex64 v = 0; v < m_transformedVertices.GetSize(); v++) {
		const Vector3f vertex = m_transformedVertices[v];
		const float distance = vertex.Dot(direction);

		if (distance > outputDistance) {
			outputDistance = distance;

			output.point = vertex;
			output.originalVertexId = v;
		}
	}

	return output;
}

DynamicArray<GjkSupport> ConvexVolume::GetAllSupports(const Vector3f& direction, float epsilon) const {
	DynamicArray<GjkSupport> output{};
	const auto support = GetSupport(direction);
	const float supportDistance = support.point.Dot(direction);

	for (UIndex64 v = 0; v < m_transformedVertices.GetSize(); v++) {
		const Vector3f& vertex = m_transformedVertices[v];
		const float distance = vertex.Dot(direction);

		if (glm::abs(distance - supportDistance) <= epsilon) {
			output.Insert({ vertex, v});
		}
	}

	return output;
}

void ConvexVolume::Transform(const Transform3D& transform) {
	m_transformedVertices.Empty();
	m_axes.Empty();

	m_transformedCenter = Vector3f::Zero;

	for (const Vector3f vertex : m_vertices) {
		const Vector3f transformedVertex = transform.TransformPoint(vertex);

		m_transformedVertices.Insert(transformedVertex);
		m_transformedCenter += transformedVertex;
	}

	m_transformedCenter /= static_cast<float>(m_transformedVertices.GetSize());

	for (UIndex32 i = 0; i < m_faces.GetSize(); i++) {
		m_axes.Insert(GetWorldSpaceAxis(i));
	}

	m_lastTransform = transform.GetAsMatrix();
}

const DynamicArray<Vector3f>& ConvexVolume::GetAxes() const {
	return m_axes;
}

const DynamicArray<Vector3f>& ConvexVolume::GetVertices() const {
	return m_transformedVertices;
}

const DynamicArray<Vector3f>& ConvexVolume::GetLocalSpaceVertices() const {
	return m_vertices;
}

DynamicArray<ConvexVolume::FaceIndices> ConvexVolume::GetFaceIndices() const {
	return m_faces;
}

UIndex64 ConvexVolume::GetFaceWithVertexIndices(std::span<const UIndex64, 3> indices, const Vector3f& refNormal) const {
	USize64 bestMatchVertexCount = 0;

	DynamicArray<UIndex64> bestMatches;

	for (UIndex64 faceId = 0; faceId < m_faces.GetSize(); faceId++) {
		const auto& face = m_faces[faceId];

		const USize64 count = 
			static_cast<USize64>(face.ContainsElement(static_cast<UIndex32>(indices[0]))) + 
			static_cast<USize64>(face.ContainsElement(static_cast<UIndex32>(indices[1]))) +
			static_cast<USize64>(face.ContainsElement(static_cast<UIndex32>(indices[2])));

		if (count == 3) {
			return faceId;
		}
		else if (count > bestMatchVertexCount) {
			bestMatches = { faceId };
			bestMatchVertexCount = count;
		}
		else if (count == bestMatchVertexCount) {
			bestMatches.Insert(faceId);
		}
	}

	UIndex64 bestMatchId = 0;
	float bestDot = std::numeric_limits<float>::lowest();

	for (UIndex64 i = 0; i < bestMatches.GetSize(); i++) {
		const Vector3f normal = m_axes[bestMatches[i]];
		const float dot = normal.Dot(refNormal);

		if (dot > bestDot) {
			bestDot = dot;
			bestMatchId = i;
		}
	}

	return bestMatches[bestMatchId];
}

RayCastResult ConvexVolume::CastRay(const Ray& ray) const {
	float closestIntersection = std::numeric_limits<float>::max();
	Vector3f intersectionPoint = Vector3f::Zero;
	bool intersection = false;

	for (UIndex64 i = 0; i < m_faces.GetSize(); i++) {
		const auto& axis = m_axes[i];
		const auto& face = m_faces[i];

		const auto planeIntersectionResult = ray.IntersectionWithPlane(axis, m_transformedVertices[face[0]]);

		if (!planeIntersectionResult.Result()) {
			continue;
		}


		// Comprobamos si está dentro de la cara
		bool insideFace = true;
		for (UIndex64 v = 0; v < face.GetSize(); v++) {
			const Vector3f pointA = m_transformedVertices[face[v]];
			const Vector3f pointB = m_transformedVertices[face[(v + 1) % face.GetSize()]];

			const Vector3f edge = pointB - pointA;

			if (axis.Dot(edge.Cross(planeIntersectionResult.GetIntersectionPoint() - pointB)) > 0.0f) {
				insideFace = false;
				break;
			}
		}

		if (insideFace) {
			intersection = true;

			const float nDistance = planeIntersectionResult.GetIntersectionPoint().GetDistanceTo2(ray.origin);
			if (nDistance < closestIntersection) {
				closestIntersection = nDistance;
				intersectionPoint = planeIntersectionResult.GetIntersectionPoint();
			}
		}
	}

	return intersection
		? RayCastResult::True(intersectionPoint, ECS::EMPTY_GAME_OBJECT)
		: RayCastResult::False();
}


template <>
nlohmann::json PERSISTENCE::SerializeData<OSK::COLLISION::ConvexVolume>(const OSK::COLLISION::ConvexVolume& data) {
	nlohmann::json output{};

	output["m_vertices"] = nlohmann::json::array();

	for (const auto& v : data.m_vertices) {
		nlohmann::json vertex{};

		vertex["x"] = v.x;
		vertex["y"] = v.y;
		vertex["z"] = v.z;

		output["m_vertices"].push_back(vertex);
	}


	output["m_faces"] = nlohmann::json::array();

	for (const auto& face : data.m_faces) {
		auto jsonFace = nlohmann::json::array();

		for (const auto i : face) {
			jsonFace.push_back(i);
		}

		output["m_faces"].push_back(jsonFace);
	}


	return output;
}

template <>
OSK::COLLISION::ConvexVolume PERSISTENCE::DeserializeData<OSK::COLLISION::ConvexVolume>(const nlohmann::json& json) {
	ConvexVolume output{};

	for (const auto& vertex : json["m_vertices"]) {
		const auto nVertex = Vector3f(
			vertex["x"],
			vertex["y"],
			vertex["z"]
		);

		output.m_vertices.Insert(nVertex);
		output.m_transformedVertices.Insert(nVertex);
	}

	for (const auto& face : json["m_faces"]) {
		ConvexVolume::FaceIndices indices{};

		for (const UIndex32 i : face) {
			indices.Insert(i);
		}

		output.m_faces.Insert(indices);
	}

	return output;
}



template <>
BinaryBlock PERSISTENCE::BinarySerializeData<OSK::COLLISION::ConvexVolume>(const OSK::COLLISION::ConvexVolume& data) {
	BinaryBlock output{};

	output.Write<USize64>(data.m_vertices.GetSize());
	for (const auto& v : data.m_vertices) {
		output.AppendBlock(SerializeBinaryVector3<Vector3f>(v));
	}

	output.Write<USize64>(data.m_faces.GetSize());
	for (const auto& face : data.m_faces) {
		auto jsonFace = nlohmann::json::array();

		output.Write<USize64>(face.GetSize());
		for (const auto i : face) {
			output.Write<UIndex32>(i);
		}
	}

	return output;
}

template <>
OSK::COLLISION::ConvexVolume PERSISTENCE::BinaryDeserializeData<OSK::COLLISION::ConvexVolume>(BinaryBlockReader* reader) {
	ConvexVolume output{};

	const USize64 numVertices = reader->Read<USize64>();

	for (UIndex64 i = 0; i < numVertices; i++) {
		const auto nVertex = DeserializeBinaryVector3<Vector3f, float>(reader);

		output.m_vertices.Insert(nVertex);
		output.m_transformedVertices.Insert(nVertex);
	}

	const USize64 numFaces = reader->Read<USize64>();

	for (UIndex64 i = 0; i < numFaces; i++) {
		const USize64 numIndices = reader->Read<USize64>();

		ConvexVolume::FaceIndices indices{};

		for (UIndex64 idx = 0; idx < numIndices; idx++) {
			indices.Insert(reader->Read<UIndex32>());
		}

		output.m_faces.Insert(indices);
	}

	return output;
}
