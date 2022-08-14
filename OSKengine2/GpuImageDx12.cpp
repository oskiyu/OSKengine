#include "GpuImageDx12.h"

#include "WindowsUtils.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageDx12::GpuImageDx12(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format, TSize numSamples, GpuImageSamplerDesc samplerDesc)
	: GpuImage(size, dimension, usage, numLayers, format, numSamples, samplerDesc) {

}

void GpuImageDx12::SetResource(const ComPtr<ID3D12Resource>& resource) {
	this->resource = resource;
}

void GpuImageDx12::_SetSampledDescriptorHeap(ComPtr<ID3D12DescriptorHeap> descriptorHeap) {
	sampledDescriptorHeap = descriptorHeap;
}

void GpuImageDx12::_SetRenderTargetDescriptorHeap(ComPtr<ID3D12DescriptorHeap> descriptorHeap) {
	renderTargetDescriptorHeap = descriptorHeap;
	colorUsageDescriptorHandle = GetRenderTargetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
}

void GpuImageDx12::_SetDepthDescriptorHeap(ComPtr<ID3D12DescriptorHeap> descriptorHeap) {
	depthDescriptorHeap = descriptorHeap;
	colorUsageDescriptorHandle = GetDepthDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
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

D3D12_CPU_DESCRIPTOR_HANDLE GpuImageDx12::GetColorUsageDescriptorHandle() const {
	return colorUsageDescriptorHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE GpuImageDx12::GetDepthUsageDescriptorHandle() const {
	return depthUsageDescriptorHandle;
}

void GpuImageDx12::SetDebugName(const std::string& name) {
	auto str = StringToWideString(name);
	resource->SetName(str.c_str());
}
