#pragma once

#include "DX12.h"
#include "Device.h"

class Fence {

public:

	void Create(Device* device) {
		auto result = device->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		OSK_ASSERT2(IsDxSuccess(result), "Error al crear el fence.", result);

		event = CreateEvent(NULL, FALSE, FALSE, NULL);
		OSK_ASSERT2(event, "Error al crear evento de fence.", result);
	}

	uint64_t Signal(ID3D12CommandQueue* commandBuffer, uint64_t& targetValue) {
		uint64_t fenceValueForSignal = ++targetValue;
		
		auto result = commandBuffer->Signal(fence.Get(), fenceValueForSignal);
		OSK_ASSERT2(IsDxSuccess(result), "Error al notificar la fence en la GPU.", result);

		return fenceValueForSignal;
	}

	void Wait(uint64_t targetValue) {
		if (fence->GetCompletedValue() < targetValue) {
			auto result = fence->SetEventOnCompletion(targetValue, event);
			OSK_ASSERT2(IsDxSuccess(result), "Error al establecer event de reanudación.", result);

			WaitForSingleObject(event, DWORD_MAX);
		}
	}

	//El programa espera hasta que se complete el commandBuffer en la GPU.
	void Flush(ID3D12CommandQueue* commandBuffer, uint64_t& targetValue) {
		Signal(commandBuffer, targetValue);
		Wait(targetValue);
	}

	ID3D12Fence* GetFence() const {
		return fence.Get();
	}

	uint64_t perFrameValues[NUMBER_OF_FRAMES]{};

	uint64_t value = 0;

private:

	ComPtr<ID3D12Fence> fence;
	
	//Para que la CPU espere, OS.
	HANDLE event;


};
