#include "GpuImageDx12.h"

using namespace OSK;

GpuImageDx12::GpuImageDx12(unsigned int sizeX, unsigned int sizeY, Format format) 
	: GpuImage(sizeX, sizeY, format) {

}

void GpuImageDx12::SetResource(const ComPtr<ID3D12Resource>& resource) {
	this->resource = ComPtr<ID3D12Resource>(resource);
}

ID3D12Resource* GpuImageDx12::GetResource() const {
	return resource.Get();
}
