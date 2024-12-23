#include "DescriptorHeapDx12.h"

#ifdef OSK_USE_DIRECTX12_BACKEND

#include "GpuDx12.h"

OSK::GRAPHICS::DescriptorHeapDx12::DescriptorHeapDx12(D3D12_DESCRIPTOR_HEAP_TYPE type, GpuDx12* device) : m_type(type), m_maxHandleCount(1024), m_device(device){
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = type;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;
	desc.NumDescriptors = m_maxHandleCount;

	device->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap));

	m_nextHandle = m_heap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE OSK::GRAPHICS::DescriptorHeapDx12::Allocate() {
	auto output = m_nextHandle;
	m_handleCount++;

	m_nextHandle.ptr += m_device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	return output;
}

bool OSK::GRAPHICS::DescriptorHeapDx12::IsFull() const {
	return m_maxHandleCount == m_handleCount;
}

#endif
