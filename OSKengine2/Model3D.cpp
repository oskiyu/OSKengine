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

void Model3D::_SetVertexBuffer(const OwnedPtr<GRAPHICS::GpuBuffer>& vertexBuffer) {
	this->vertexBuffer = vertexBuffer.GetPointer();
}

void Model3D::_SetIndexBuffer(const OwnedPtr<GRAPHICS::GpuBuffer>& indexBuffer) {
	this->indexBuffer = indexBuffer.GetPointer();
}

void Model3D::_SetAccelerationStructure(OwnedPtr<GRAPHICS::IBottomLevelAccelerationStructure> accelerationStructure) {
	this->accelerationStructure = accelerationStructure.GetPointer();
}

void Model3D::_SetIndexCount(USize32 count) {
	numIndices = count;
}

GpuBuffer* Model3D::GetVertexBuffer() const {
	return vertexBuffer.GetPointer();
}

GpuBuffer* Model3D::GetIndexBuffer() const {
	return indexBuffer.GetPointer();
}

IBottomLevelAccelerationStructure* Model3D::GetAccelerationStructure() const {
	return accelerationStructure.GetPointer();
}

USize32 Model3D::GetIndexCount() const {
	return numIndices;
}

void Model3D::AddGpuImage(OwnedPtr<GRAPHICS::GpuImage> image) {
	metadata.textures.Insert(image.GetPointer());
}

const GpuImage* Model3D::GetImage(UIndex32 index) const {
	return metadata.textures.At(index).GetPointer();
}

const ModelMetadata& Model3D::GetMetadata() const {
	return metadata;
}

void Model3D::_SetAnimator(GRAPHICS::Animator&& animator) {
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