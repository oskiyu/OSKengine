#include "Model3D.h"

using namespace OSK;
using namespace OSK::ASSETS;

Model3D::Model3D(const std::string& assetFile)
	: IAsset(assetFile) {

}

void Model3D::_SetVertexBuffer(const OwnedPtr<GRAPHICS::IGpuVertexBuffer>& vertexBuffer) {
	this->vertexBuffer = vertexBuffer;
}

void Model3D::_SetIndexBuffer(const OwnedPtr<GRAPHICS::IGpuIndexBuffer>& indexBuffer) {
	this->indexBuffer = indexBuffer;
}

void Model3D::_SetIndexCount(TSize count) {
	numIndices = count;
}

GRAPHICS::IGpuVertexBuffer* Model3D::GetVertexBuffer() const {
	return vertexBuffer.GetPointer();
}

GRAPHICS::IGpuIndexBuffer* Model3D::GetIndexBuffer() const {
	return indexBuffer.GetPointer();
}

TSize Model3D::GetIndexCount() const {
	return numIndices;
}
