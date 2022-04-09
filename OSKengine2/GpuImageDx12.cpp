#include "GpuImageDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageDx12::GpuImageDx12(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format)
	: GpuImage(size, dimension, usage, numLayers, format) {

}

void GpuImageDx12::SetResource(const ComPtr<ID3D12Resource>& resource) {
	this->resource = resource;
}

void GpuImageDx12::_SetSampledDescriptorHeap(ComPtr<ID3D12DescriptorHeap> descriptorHeap) {
	sampledDescriptorHeap = descriptorHeap;
}

void GpuImageDx12::_SetRenderTargetDescriptorHeap(ComPtr<ID3D12DescriptorHeap> descriptorHeap) {
	renderTargetDescriptorHeap = descriptorHeap;
}

void GpuImageDx12::_SetDepthDescriptorHeap(ComPtr<ID3D12DescriptorHeap> descriptorHeap) {
	depthDescriptorHeap = descriptorHeap;
}

ID3D12Resource* GpuImageDx12::GetResource() const {
	return resource.Get();
}

ID3D12DescriptorHeap* GpuImageDx12::GetSampledDescriptorHeap() const {
	return sampledDescriptorHeap.Get();
}

ID3D12DescriptorHeap* GpuImageDx12::GetRenderTargetDescriptorHeap() const {
	return renderTargetDescriptorHeap.Get();
}

ID3D12DescriptorHeap* GpuImageDx12::GetDepthDescriptorHeap() const {
	return depthDescriptorHeap.Get();
}