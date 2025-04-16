#include "GpuModel3D.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


void GpuModel3D::SetUuid(GpuModelUuid uuid) {
	m_uuid = uuid;
}

GpuModelUuid GpuModel3D::GetUuid() const {
	return m_uuid;
}

void GpuModel3D::SetCpuModel3D(const CpuModel3D& model) {
	m_cpuModel = model;
}

CpuModel3D& GpuModel3D::GetCpuModel() {
	return m_cpuModel;
}

const CpuModel3D& GpuModel3D::GetCpuModel() const {
	return m_cpuModel;
}

void GpuModel3D::SetVertexBuffer(UniquePtr<GpuBuffer>&& vertexBuffer) {
	m_vertexBuffer = std::move(vertexBuffer);
}

void GpuModel3D::SetIndexBuffer(UniquePtr<GpuBuffer>&& indexBuffer) {
	m_indexBuffer = std::move(indexBuffer);
}

void GpuModel3D::SetIndexCount(USize32 indexCount) {
	m_indexCount = indexCount;
}

void GpuModel3D::AddMesh(const GpuMesh3D& mesh) {
	m_meshes.Insert(mesh);
}

const DynamicArray<GpuMesh3D>& GpuModel3D::GetMeshes() const {
	return m_meshes;
}

DynamicArray<GpuMesh3D>& GpuModel3D::GetMeshes() {
	return m_meshes;
}


void GpuModel3D::EstablisSingleLod() {
	m_levelsOfDetail = {
		Lod {
			.firstMeshId = 0,
			.meshesCount = m_meshes.GetSize()
		}
	};
}

void GpuModel3D::AddLod(const Lod& lod) {
	m_levelsOfDetail.Insert(lod);
}

const GpuModel3D::Lod& GpuModel3D::GetLod(UIndex64 lod) const {
	return m_levelsOfDetail[lod];
}

const GpuModel3D::Lod& GpuModel3D::GetLodOrMin(UIndex64 lod) const {
	if (lod == 0) {
		return m_levelsOfDetail[0];
	}

	return lod < m_levelsOfDetail.GetSize()
		? m_levelsOfDetail[lod]
		: m_levelsOfDetail[m_levelsOfDetail.GetSize() - 1];
}

void GpuModel3D::SetMaterials(const DynamicArray<Material>& materials) {
	m_materials = materials;
}

GpuModel3D::Material& GpuModel3D::GetMaterial(UIndex64 materialIndex) {
	OSK_ASSERT(materialIndex < m_materials.GetSize(), InvalidArgumentException("No existe el material indicado."));
	return m_materials[materialIndex];
}

const GpuModel3D::Material& GpuModel3D::GetMaterial(UIndex64 materialIndex) const {
	OSK_ASSERT(materialIndex < m_materials.GetSize(), InvalidArgumentException("No existe el material indicado."));
	return m_materials[materialIndex];
}

const GpuBuffer& GpuModel3D::GetVertexBuffer() const {
	return m_vertexBuffer.GetValue();
}

const GpuBuffer& GpuModel3D::GetIndexBuffer() const {
	return m_indexBuffer.GetValue();
}

USize32 GpuModel3D::GetTotalIndexCount() const {
	return m_indexCount;
}

GpuModel3D::TextureTable& GpuModel3D::GetTextureTable() {
	return m_textures;
}

const GpuModel3D::TextureTable& GpuModel3D::GetTextureTable() const {
	return m_textures;
}

VerticesAttributesMaps& GpuModel3D::GetVertexAttributesMap() {
	return m_vertexAttributes;
}

const VerticesAttributesMaps& GpuModel3D::GetVertexAttributesMap() const {
	return m_vertexAttributes;
}

bool GpuModel3D::HasAnimator() const {
	return m_animator.has_value();
}

Animator& GpuModel3D::GetAnimator() {
	return m_animator.value();
}

const Animator& GpuModel3D::GetAnimator() const {
	return m_animator.value();
}

void GpuModel3D::SetAnimator(Animator&& animator) {
	m_animator = std::move(animator);
}


// --- TextureTable --- //

const IGpuImageView* GpuModel3D::TextureTable::GetImageView(UIndex64 imageIndex) const {
	if (m_textures.contains(imageIndex)) {
		return &m_textures.at(imageIndex)->GetTextureView();
	}

	if (m_imageViews.contains(imageIndex)) {
		return m_imageViews.at(imageIndex);
	}

	OSK_ASSERT(
		false,
		InvalidArgumentException(std::format("No existe el image view en el índice {}.", imageIndex)));
}

void GpuModel3D::TextureTable::SetTexture(UIndex64 index, ASSETS::AssetRef<ASSETS::Texture> texture) {
	m_textures[index] = texture;
}

void GpuModel3D::TextureTable::SetGpuImageView(UIndex64 index, const IGpuImageView* view) {
	m_imageViews[index] = view;
}


void GpuModel3D::TextureTable::AddTexture(ASSETS::AssetRef<ASSETS::Texture> texture) {
	m_textures[m_nextIndex] = texture;
	m_nextIndex++;
}

void GpuModel3D::TextureTable::AddGpuImageView(const IGpuImageView* view) {
	m_imageViews[m_nextIndex] = view;
	m_nextIndex++;
}
