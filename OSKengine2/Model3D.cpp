#include "Model3D.h"

#include "Mesh3D.h"
#include "IGpuImage.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

Model3D::Model3D(const std::string& assetFile)
	: IAsset(assetFile) {

}
const Model3D::Lod& Model3D::GetLod(UIndex64 level) {
	return m_lods[glm::min(level, GetLowestLod())];
}

UIndex64 Model3D::GetHighestLod() const {
	return Lod::HighestLevel;
}

UIndex64 Model3D::GetLowestLod() const {
	return m_lods.GetSize() - 1;
}

void Model3D::_RegisterLod(const Lod& lod) {
	m_lods.Insert(lod);
}


void Model3D::_SetId(UIndex64 id) {
	m_modelId = id;
}

UIndex64 Model3D::GetId() const {
	return m_modelId;
}

const DynamicArray<Mesh3D>& Model3D::GetMeshes() const {
	return m_meshes;
}

DynamicArray<Mesh3D>& Model3D::GetMeshes() {
	return m_meshes;
}

void Model3D::AddMesh(const Mesh3D& mesh, const MeshMetadata& meshMetadata) {
	m_meshes.Insert(mesh);
	m_metadata.meshesMetadata.Insert(meshMetadata);
}
void Model3D::SetRenderPassType(const std::string& renderPassType) {
	m_renderPassType = renderPassType;
}

std::string_view Model3D::GetRenderPassType() const {
	return m_renderPassType;
}

void Model3D::_SetVertexBuffer(const OwnedPtr<GRAPHICS::GpuBuffer>& vertexBuffer) {
	this->vertexBuffer = vertexBuffer.GetPointer();
}

void Model3D::_SetIndexBuffer(const OwnedPtr<GRAPHICS::GpuBuffer>& indexBuffer) {
	this->indexBuffer = indexBuffer.GetPointer();
}

void Model3D::_SetIndexCount(USize32 count) {
	m_numIndices = count;
}

USize32 Model3D::GetIndexCount() const {
	return m_numIndices;
}

void Model3D::AddGpuImage(OwnedPtr<GRAPHICS::GpuImage> image) {
	m_metadata.textures.Insert(image.GetPointer());
}

void Model3D::_AddMaterialBuffer(OwnedPtr<GRAPHICS::GpuBuffer> buffer) {
	m_metadata.materialInfos.Insert(buffer.GetPointer());
}

const GpuImage* Model3D::GetImage(UIndex32 index) const {
	return m_metadata.textures.At(index).GetPointer();
}

const ModelMetadata& Model3D::GetMetadata() const {
	return m_metadata;
}

void Model3D::_SetAnimator(GRAPHICS::Animator&& animator) {
	m_animator = new Animator(std::move(animator));
}

ModelType Model3D::GetType() const {
	return GetAnimator() == nullptr
			? ModelType::STATIC_MESH
			: ModelType::ANIMATED_MODEL;
}
