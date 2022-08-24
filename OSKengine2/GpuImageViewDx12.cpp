#include "GpuImageViewDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageViewDx12::GpuImageViewDx12(DescriptorDx12 descriptor, SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage)
	: IGpuImageView(channel, arrayType, baseArrayLevel, layerCount, usage), descriptor(descriptor) {

}

D3D12_CPU_DESCRIPTOR_HANDLE GpuImageViewDx12::GetCpuAddress() const {
	return descriptor.cpuHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE GpuImageViewDx12::GetGpuAddress() const {
	return descriptor.gpuHandle;
}

ID3D12DescriptorHeap* GpuImageViewDx12::GetDescriptorHeap() const {
	return descriptor.heap;
}
