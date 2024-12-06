#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "DynamicArray.hpp"

#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include <d3d12.h>

namespace OSK::GRAPHICS {

	class GpuDx12;


	struct DescriptorDx12 {
		ID3D12DescriptorHeap* heap = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{ NULL };
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{ NULL };
	};


	/// <summary>
	/// Estructura para ayudar al manejo de descriptor heaps.
	/// </summary>
	class DescriptorBlockDx12 {

	public:

		DescriptorBlockDx12(D3D12_DESCRIPTOR_HEAP_TYPE type, USize32 numDescriptors, GpuDx12* gpu);

		ID3D12DescriptorHeap* GetHeap() const;
		USize32 GetNumRemainingDescriptors() const;

		bool IsFull() const;

		DescriptorDx12 CreateDescriptor();

	private:

		/// <summary>
		///  Heap con capacidad para almacenar descriptors de un tipo dado.
		/// </summary>
		ComPtr<ID3D12DescriptorHeap> heap;

		/// <summary>
		/// Número de descriptors libres dentro de este bloque.
		/// </summary>
		USize32 remainingDescriptors = 0;

		/// <summary>
		/// Índice del próximo descriptor a crear.
		/// Empieza en 0, y aumenta en 1 cada vez que se crea un descriptor.
		/// </summary>
		UIndex32 nextDescriptorIndex = 0;

		GpuDx12* gpu = nullptr;
		D3D12_DESCRIPTOR_HEAP_TYPE type{};

	};

}

#endif
