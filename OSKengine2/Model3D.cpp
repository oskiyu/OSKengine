#include "Model3D.h"

#include "Mesh3D.h"
#include "IGpuImage.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

Model3D::Model3D(const std::string& assetFile)
	: IAsset(assetFile) {

}

const DynamicArray<Mesh3D>& Model3D::GetMeshes() const {
	return meshes;
}

void Model3D::AddMesh(const Mesh3D& mesh, const MeshMetadata& meshMetadata) {
	meshes.Insert(mesh);
	metadata.meshesMetadata.Insert(meshMetadata);
}

void Model3D::_SetVertexBuffer(const OwnedPtr<GRAPHICS::IGpuVertexBuffer>& vertexBuffer) {
	this->vertexBuffer = vertexBuffer.GetPointer();
}

void Model3D::_SetIndexBuffer(const OwnedPtr<GRAPHICS::IGpuIndexBuffer>& indexBuffer) {
	this->indexBuffer = indexBuffer.GetPointer();
}

void Model3D::_SetAccelerationStructure(const OwnedPtr<GRAPHICS::IBottomLevelAccelerationStructure>& accelerationStructure) {
	this->accelerationStructure = accelerationStructure.GetPointer();
}

void Model3D::_SetIndexCount(TSize count) {
	numIndices = count;
}

IGpuVertexBuffer* Model3D::GetVertexBuffer() const {
	return vertexBuffer.GetPointer();
}

IGpuIndexBuffer* Model3D::GetIndexBuffer() const {
	return indexBuffer.GetPointer();
}

IBottomLevelAccelerationStructure* Model3D::GetAccelerationStructure() const {
	return accelerationStructure.GetPointer();
}

TSize Model3D::GetIndexCount() const {
	return numIndices;
}

void Model3D::AddGpuImage(OwnedPtr<GRAPHICS::GpuImage> image) {
	metadata.textures.Insert(image.GetPointer());
}

const GpuImage* Model3D::GetImage(TSize index) const {
	return metadata.textures.At(index).GetPointer();
}

const ModelMetadata& Model3D::GetMetadata() const {
	return metadata;
}

void Model3D::SetAnimator(GRAPHICS::Animator&& animator) {
	this->animator = new Animator(std::move(animator));
}

Animator* Model3D::GetAnimator() const {
	return animator.GetPointer();
}

ModelType Model3D::GetType() const {
	return GetAnimator() == nullptr
			? ModelType::STATIC_MESH
			: ModelType::ANIMATED_MODEL;
}