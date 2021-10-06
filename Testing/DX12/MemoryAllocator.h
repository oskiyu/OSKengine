#pragma once

#include "DX12.h"

#include "Device.h"
#include "GpuImage.h"

class MemoryAllocator {

public:

	void Create(Device* device) {
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = 1024;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		auto result = device->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&renderTargetDescriptorMemory));
		OSK_ASSERT2(IsDxSuccess(result), "Error al crear memoria.", result);
		SET_DX12_OBJECT_NAME(renderTargetDescriptorMemory);

		renderTargetMemorySize = device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1024;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		result = device->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&depthDescriptorMemory));
		OSK_ASSERT2(IsDxSuccess(result), "Error al crear memoria.", result);
		SET_DX12_OBJECT_NAME(depthDescriptorMemory);
	}

	void CreateDepthImage(Image* image, DXGI_FORMAT format, uint32_t sizeX, uint32_t sizeY) {
		D3D12_HEAP_PROPERTIES depthMemoryProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC depthResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, sizeX, sizeY, 1, 1);
		depthResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		CD3DX12_CLEAR_VALUE clearValue(format, 1.0f, 0);

		auto result = device->GetDevice()->CreateCommittedResource(
			&depthMemoryProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&image->gpuImage)
		);
		OSK_ASSERT2(IsDxSuccess(result), "Error al crear imagen de profundidad.", result);

		D3D12_DEPTH_STENCIL_VIEW_DESC depthViewDesc = {};
		depthViewDesc.Format = format;
		depthViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthViewDesc.Flags = D3D12_DSV_FLAG_NONE;

		device->GetDevice()->CreateDepthStencilView(image->gpuImage.Get(), &depthViewDesc, depthDescriptorMemory->GetCPUDescriptorHandleForHeapStart());
	}

	ID3D12DescriptorHeap* GetRenderTargetDescriptorMemory() const {
		return renderTargetDescriptorMemory.Get();
	}

	uint32_t GetRenderTargetDescriptorMemorySize() const {
		return renderTargetMemorySize;
	}

	ID3D12DescriptorHeap* GetDepthDescriptorMemory() const {
		return depthDescriptorMemory.Get();
	}

private:

	ComPtr<ID3D12DescriptorHeap> renderTargetDescriptorMemory;
	uint32_t renderTargetMemorySize;

	ComPtr<ID3D12DescriptorHeap> depthDescriptorMemory;

	Device* device = nullptr;

};
