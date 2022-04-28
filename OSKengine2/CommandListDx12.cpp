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
#include "GraphicsPipelineDx12.h"
#include "GpuVertexBufferDx12.h"
#include "Viewport.h"
#include "GpuIndexBufferDx12.h"
#include "MaterialSlotDx12.h"
#include "Material.h"
#include "FormatDx12.h"
#include "Format.h"
#include "MaterialLayout.h"
#include "Math.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

TSize Multiplo256(TSize original) {
	TSize output = original / 256;
	if (original % 256 != 0)
		output++;

	return output * 256;
}

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

void CommandListDx12::TransitionImageLayout(GpuImage* image, GpuImageLayout previous, GpuImageLayout next, TSize baseLayer, TSize numLayers) {
	ID3D12Resource* resource = image->As<GpuImageDx12>()->GetResource();
	
	D3D12_RESOURCE_BARRIER barrierInfo{};
	barrierInfo.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierInfo.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrierInfo.Transition.pResource = resource;
	barrierInfo.Transition.StateBefore = GetGpuImageLayoutDx12(previous);
	barrierInfo.Transition.StateAfter = GetGpuImageLayoutDx12(next);
	barrierInfo.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrierInfo);

	image->SetLayout(next);
}

void CommandListDx12::CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest, TSize layer, TSize offset) {
	D3D12_TEXTURE_COPY_LOCATION copyDest{};
	copyDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	copyDest.SubresourceIndex = layer;
	copyDest.pResource = dest->GetBuffer()->As<GpuMemorySubblockDx12>()->GetResource();

	D3D12_TEXTURE_COPY_LOCATION copySource{};
	copySource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	copySource.pResource = source->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource();
	copySource.PlacedFootprint.Footprint.Depth = 1;
	copySource.PlacedFootprint.Footprint.Format = GetFormatDx12(dest->GetFormat());
	copySource.PlacedFootprint.Footprint.Width = dest->GetSize().X;
	copySource.PlacedFootprint.Footprint.Height = dest->GetSize().Y;
	copySource.PlacedFootprint.Footprint.RowPitch = dest->GetSize().X * GetFormatNumberOfBytes(dest->GetFormat());
	copySource.PlacedFootprint.Offset = offset;

	commandList->CopyTextureRegion(&copyDest, 0, 0, 0, &copySource, nullptr);
}

void CommandListDx12::BeginRenderpass(IRenderpass* renderpass) {
	BeginAndClearRenderpass(renderpass, Color::BLACK());
}

void CommandListDx12::BeginAndClearRenderpass(IRenderpass* renderpass, const Color& color) {
	const auto size = renderpass->GetImage(0)->GetSize();

	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDesc{};
	if (renderpass->GetType() == RenderpassType::FINAL) {
		const SwapchainDx12* swapchain = renderpass->As<RenderpassDx12>()->GetSwapchain();

		renderTargetDesc = swapchain->GetRenderTargetMemory()->GetCPUDescriptorHandleForHeapStart();
		renderTargetDesc.ptr += ((SIZE_T)swapchain->GetCurrentFrameIndex()) *
			Engine::GetRenderer()->As<RendererDx12>()->GetGpu()->As<GpuDx12>()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		depthStencilDesc = swapchain->GetDepthStencilMemory()->GetCPUDescriptorHandleForHeapStart();
		depthStencilDesc.ptr += ((SIZE_T)swapchain->GetCurrentFrameIndex()) *
			Engine::GetRenderer()->As<RendererDx12>()->GetGpu()->As<GpuDx12>()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}
	else {
		renderTargetDesc = renderpass->As<RenderpassDx12>()->GetRenderTargetDescpriptor(0);
		depthStencilDesc = renderpass->As<RenderpassDx12>()->GetDepthStencilDescpriptor(0);
	}

	As<ICommandList>()->TransitionImageLayout(renderpass->GetImage(renderpass->As<RenderpassDx12>()->GetSwapchain()->GetCurrentFrameIndex()), GpuImageLayout::COLOR_ATTACHMENT, 0, 1);

	const FLOAT clearValue[] = { color.Red, color.Green, color.Blue, color.Alpha };
	commandList->ClearRenderTargetView(renderTargetDesc, clearValue, 0, nullptr);
	commandList->ClearDepthStencilView(depthStencilDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	commandList->OMSetRenderTargets(1, &renderTargetDesc, FALSE, &depthStencilDesc);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	currentRenderpass = renderpass;
}

void CommandListDx12::EndRenderpass(IRenderpass* renderpass) {
	GpuImageLayout finalLayout = GpuImageLayout::SHADER_READ_ONLY;
	if (renderpass->GetType() == RenderpassType::FINAL)
		finalLayout = GpuImageLayout::PRESENT;

	As<ICommandList>()->TransitionImageLayout(renderpass->GetImage(renderpass->As<RenderpassDx12>()->GetSwapchain()->GetCurrentFrameIndex()), finalLayout, 0, 1);
}

void CommandListDx12::ResourceBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) {
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource;
	barrier.Transition.StateBefore = from;
	barrier.Transition.StateAfter = to;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrier);
}

void CommandListDx12::BindMaterial(const Material* material) {
	currentMaterial = material;
	currentPipeline = material->GetGraphicsPipeline(currentRenderpass);
	
	commandList->SetGraphicsRootSignature(currentPipeline->As<GraphicsPipelineDx12>()->GetLayout());
	commandList->SetPipelineState(currentPipeline->As<GraphicsPipelineDx12>()->GetPipelineState());
}

void CommandListDx12::BindVertexBuffer(IGpuVertexBuffer* buffer) {
	D3D12_VERTEX_BUFFER_VIEW views[] = {
		buffer->As<GpuVertexBufferDx12>()->GetView()
	};

	commandList->IASetVertexBuffers(0, 1, views);
}

void CommandListDx12::BindIndexBuffer(IGpuIndexBuffer* buffer) {
	commandList->IASetIndexBuffer(buffer->As<GpuIndexBufferDx12>()->GetView());
}

void CommandListDx12::BindMaterialSlot(const IMaterialSlot* slot) {
	for (const auto& i : slot->As<MaterialSlotDx12>()->GetUniformBuffers())
		if (i.second != nullptr)
			BindUniformBufferDx12(i.first, i.second);

	for (const auto& i : slot->As<MaterialSlotDx12>()->GetGpuImages())
		if (i.second != nullptr)
			BindImageDx12(i.first, i.second);
}

void CommandListDx12::PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) {
	auto& pushConst = currentMaterial->GetLayout()->GetPushConstant(pushConstName);

	TSize nSize = size / 4;
	if (size % 4)
		nSize++;

	commandList->SetGraphicsRoot32BitConstants(pushConst.hlslBindingIndex, nSize, data, pushConst.offset + offset);
}

void CommandListDx12::BindUniformBufferDx12(TSize index, const GpuUniformBufferDx12* buffer) {
	commandList->SetGraphicsRootConstantBufferView(index,
		buffer->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress());
}

void CommandListDx12::BindImageDx12(TSize index, const GpuImageDx12* image) {
	ID3D12DescriptorHeap* heaps[] = { image->GetSampledDescriptorHeap() };

	commandList->SetDescriptorHeaps(1, heaps);
	commandList->SetGraphicsRootDescriptorTable(index, heaps[0]->GetGPUDescriptorHandleForHeapStart());
}

void CommandListDx12::DrawSingleInstance(TSize numIndices) {
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

void CommandListDx12::DrawSingleMesh(TSize firstIndex, TSize numIndices) {
	commandList->DrawIndexedInstanced(numIndices, 1, firstIndex, 0, 0);
}

void CommandListDx12::SetViewport(const Viewport& vp) {
	D3D12_VIEWPORT viewport{};

	viewport.TopLeftX = vp.rectangle.GetRectanglePosition().X;
	viewport.TopLeftY = vp.rectangle.GetRectanglePosition().Y;
	viewport.Width = vp.rectangle.GetRectangleSize().X;
	viewport.Height = vp.rectangle.GetRectangleSize().Y;

	viewport.MinDepth = vp.minDepth;
	viewport.MaxDepth = vp.maxDepth;

	commandList->RSSetViewports(1, &viewport);
}

void CommandListDx12::SetScissor(const Vector4ui& scissor) {
	D3D12_RECT scissorRect{};

	scissorRect.left = scissor.GetRectanglePosition().X;
	scissorRect.top = scissor.GetRectanglePosition().Y;
	scissorRect.right = scissor.GetRectangleSize().X;
	scissorRect.bottom = scissor.GetRectangleSize().Y;

	commandList->RSSetScissorRects(1, &scissorRect);
}
