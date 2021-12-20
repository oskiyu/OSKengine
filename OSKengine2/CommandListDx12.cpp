#include "CommandListDx12.h"

#include "CommandPoolDx12.h"
#include "GpuMemoryBlockDx12.h"
#include "GpuMemorySubblockDx12.h"
#include "GpuImageLayoutDx12.h"
#include "IGpuImage.h"
#include "GpuImageDx12.h"
#include "Color.hpp"
#include "RenderpassDx12.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"
#include "RenderpassType.h"
#include "SwapchainDx12.h"
#include "OSKengine.h"
#include "RendererDx12.h"
#include "GpuDx12.h"

using namespace OSK;

void CommandListDx12::SetCommandList(const ComPtr<ID3D12GraphicsCommandList>& commandList) {
	this->commandList = commandList;
}

ID3D12GraphicsCommandList* CommandListDx12::GetCommandList() const {
	return commandList.Get();
}

void CommandListDx12::SetCommandPool(const CommandPoolDx12& commandPool) {
	this->commandPool = &commandPool;
}

void CommandListDx12::Reset() {
	commandPool->GetCommandAllocator()->Reset();
	commandList->Reset(commandPool->GetCommandAllocator(), nullptr);
}

void CommandListDx12::Start() {
	
}

void CommandListDx12::Close() {
	commandList->Close();
}

void CommandListDx12::TransitionImageLayout(GpuImage* image, GpuImageLayout next) {
	ID3D12Resource* resource = image->As<GpuImageDx12>()->GetResource();
	
	D3D12_RESOURCE_BARRIER barrierInfo{};
	barrierInfo.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierInfo.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrierInfo.Transition.pResource = resource;
	barrierInfo.Transition.StateBefore = GetGpuImageLayoutDx12(image->GetLayout());
	barrierInfo.Transition.StateAfter = GetGpuImageLayoutDx12(next);
	barrierInfo.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrierInfo);

	image->SetLayout(next);
}

void CommandListDx12::BeginRenderpass(IRenderpass* renderpass) {
	BeginAndClearRenderpass(renderpass, Color::BLACK());
}

void CommandListDx12::BeginAndClearRenderpass(IRenderpass* renderpass, const Color& color) {
	const auto size = renderpass->GetImage(0)->GetSize();

	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDesc;
	if (renderpass->GetType() == RenderpassType::FINAL) {
		const SwapchainDx12* swapchain = renderpass->As<RenderpassDx12>()->GetSwapchain();

		renderTargetDesc = renderpass->As<RenderpassDx12>()->GetSwapchain()->GetRenderTargetMemory()->GetCPUDescriptorHandleForHeapStart();
		renderTargetDesc.ptr += ((SIZE_T)swapchain->GetCurrentFrameIndex()) *
			Engine::GetRenderer()->As<RendererDx12>()->GetGpu()->As<GpuDx12>()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		depthStencilDesc = renderpass->As<RenderpassDx12>()->GetDepthStencilDescpriptor(swapchain->GetCurrentFrameIndex());
	}
	else {
		renderTargetDesc = renderpass->As<RenderpassDx12>()->GetRenderTargetDescpriptor(0);
		depthStencilDesc = renderpass->As<RenderpassDx12>()->GetDepthStencilDescpriptor(0);
	}

	commandList->OMSetRenderTargets(1, &renderTargetDesc, FALSE, nullptr);

	TransitionImageLayout(renderpass->GetImage(renderpass->As<RenderpassDx12>()->GetSwapchain()->GetCurrentFrameIndex()), GpuImageLayout::COLOR_ATTACHMENT);

	const FLOAT clearValue[] = { color.Red, color.Green, color.Blue, color.Alpha };
	commandList->ClearRenderTargetView(renderTargetDesc, clearValue, 0, nullptr);
	//commandList->ClearDepthStencilView(depthStencilDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void CommandListDx12::EndRenderpass(IRenderpass* renderpass) {
	GpuImageLayout finalLayout = GpuImageLayout::SHADER_READ_ONLY;
	if (renderpass->GetType() == RenderpassType::FINAL)
		finalLayout = GpuImageLayout::PRESENT;

	TransitionImageLayout(renderpass->GetImage(renderpass->As<RenderpassDx12>()->GetSwapchain()->GetCurrentFrameIndex()), finalLayout);
}
