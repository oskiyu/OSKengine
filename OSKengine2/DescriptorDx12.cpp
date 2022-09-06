#include "DescriptorDx12.h"

#include "GpuDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

DescriptorBlockDx12::DescriptorBlockDx12(D3D12_DESCRIPTOR_HEAP_TYPE type, TSize numDescriptors, GpuDx12* gpu) : gpu(gpu) {
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Type = type;

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV || type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	else
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	gpu->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&heap));
}

DescriptorDx12 DescriptorBlockDx12::CreateDescriptor() {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuview = heap->GetCPUDescriptorHandleForHeapStart();
	cpuview.ptr += gpu->GetDevice()->GetDescriptorHandleIncrementSize(type) * nextDescriptorIndex;

	D3D12_GPU_DESCRIPTOR_HANDLE gpuview = heap->GetGPUDescriptorHandleForHeapStart();
	gpuview.ptr += gpu->GetDevice()->GetDescriptorHandleIncrementSize(type) * nextDescriptorIndex;

	nextDescriptorIndex++;

	return DescriptorDx12 {
		.heap = GetHeap(),
		.cpuHandle = cpuview,
		.gpuHandle = gpuview
	};
}

ID3D12DescriptorHeap* DescriptorBlockDx12::GetHeap() const {
	return heap.Get();
}

TSize DescriptorBlockDx12::GetNumRemainingDescriptors() const {
	return remainingDescriptors;
}

bool DescriptorBlockDx12::IsFull() const {
	return GetNumRemainingDescriptors() > 0;
}
