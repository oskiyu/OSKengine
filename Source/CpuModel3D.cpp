#include "CpuModel3D.h"

using namespace OSK;


void CpuMesh3D::AddVertex(CpuVertex3D vertex) {
	m_vertices.Insert(vertex);
}

void CpuMesh3D::AddVertices(const DynamicArray<CpuVertex3D>& vertices) {
	m_vertices.InsertAll(vertices);
}


void CpuMesh3D::AddTriangleIndices(const DynamicArray<TriangleIndices>& indices) {
	m_triangles.InsertAll(indices);
}

void CpuMesh3D::AddLineIndices(const DynamicArray<LineIndices>& indices) {
	m_lines.InsertAll(indices);
}

void CpuMesh3D::AddPointIndices(const DynamicArray<PointIndex>& indices) {
	m_points.InsertAll(indices);
}


void CpuMesh3D::AddTriangleIndex(TriangleIndices indices) {
	m_triangles.Insert(indices);
}

void CpuMesh3D::AddLineIndex(LineIndices indices) {
	m_lines.Insert(indices);
}

void CpuMesh3D::AddPointIndex(PointIndex indices) {
	m_points.Insert(indices);
}


bool CpuMesh3D::HasTriangles() const {
	return !m_triangles.IsEmpty();
}

bool CpuMesh3D::HasLines() const {
	return !m_lines.IsEmpty();
}

bool CpuMesh3D::HasPoints() const {
	return !m_points.IsEmpty();
}


USize64 CpuMesh3D::GetTriangleCount() const {
	return m_triangles.GetSize();
}

USize64 CpuMesh3D::GetLineCount() const {
	return m_lines.GetSize();
}

USize64 CpuMesh3D::GetPointCount() const {
	return m_points.GetSize();
}

const DynamicArray<CpuMesh3D::TriangleIndices>& CpuMesh3D::GetTriangles() const {
	return m_triangles;
}

const DynamicArray<CpuMesh3D::LineIndices>& CpuMesh3D::GetLines() const {
	return m_lines;
}

const DynamicArray<CpuMesh3D::PointIndex>& CpuMesh3D::GetPoints() const {
	return m_points;
}

const DynamicArray<CpuVertex3D>& CpuMesh3D::GetVertices() const {
	return m_vertices;
}

void CpuMesh3D::SetMaterialIndex(UIndex64 index) {
	m_materialIndex = index;
}

bool CpuMesh3D::HasMaterial() const {
	return m_materialIndex.has_value();
}

UIndex64 CpuMesh3D::GetMaterialIndex() const {
	return m_materialIndex.value();
}


// Model //

void CpuModel3D::AddMeshes(const DynamicArray<CpuMesh3D>& meshes) {
	m_meshes.InsertAll(meshes);
}

const DynamicArray<CpuMesh3D>& CpuModel3D::GetMeshes() const {
	return m_meshes;
}

void CpuModel3D::AddAnimations(const DynamicArray<GRAPHICS::Animation>& animations) {
	m_animations.InsertAll(animations);
}

void CpuModel3D::AddAnimationSkins(const DynamicArray<GRAPHICS::AnimationSkin>& skins) {
	m_animationSkins.InsertAll(skins);
}

const DynamicArray<GRAPHICS::Animation>& CpuModel3D::GetAnimations() const {
	return m_animations;
}

const DynamicArray<GRAPHICS::AnimationSkin>& CpuModel3D::GetAnimationSkins() const {
	return m_animationSkins;
}

void CpuModel3D::SetInitialTransform(const glm::mat4& initialTransform) {
	m_initialTransform = initialTransform;
}

const glm::mat4& CpuModel3D::GetInitialTransform() const {
	return m_initialTransform;
}
