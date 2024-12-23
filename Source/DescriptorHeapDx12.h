#pragma once

#include "Platforms.h"

#ifdef OSK_USE_DIRECTX12_BACKEND

#include "ApiCall.h"
#include "NumericTypes.h"

#include <d3d12.h>
#include <wrl.h>

namespace OSK::GRAPHICS {

	class GpuDx12;

	/// @brief 
	class OSKAPI_CALL DescriptorHeapDx12 final {

	public:

		explicit DescriptorHeapDx12(D3D12_DESCRIPTOR_HEAP_TYPE type, GpuDx12* device);

		D3D12_CPU_DESCRIPTOR_HANDLE Allocate();
		bool IsFull() const;

	private:

		D3D12_DESCRIPTOR_HEAP_TYPE m_type{};
		D3D12_CPU_DESCRIPTOR_HANDLE m_startHandle{};
		D3D12_CPU_DESCRIPTOR_HANDLE m_nextHandle{};
		USize32 m_handleCount = 0;
		USize32 m_maxHandleCount = 0;

		GpuDx12* m_device =  nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;

	};

}

#endif
