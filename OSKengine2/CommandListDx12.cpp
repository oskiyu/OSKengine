#include "CommandListDx12.h"

#include "CommandPoolDx12.h"
#include "GpuMemoryBlockDx12.h"
#include "GpuMemorySubblockDx12.h"
#include "GpuImageLayoutDx12.h"
#include "IGpuImage.h"
#include "GpuImageDx12.h"
#include "Color.hpp"
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
#include "WindowsUtils.h"

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

void CommandListDx12::SetGpuImageBarrier(GpuImage* image, GpuImageLayout previousLayout, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageBarrierInfo& prevImageInfo) {
	ID3D12Resource* resource = image->As<GpuImageDx12>()->GetResource();
	
	D3D12_RESOURCE_BARRIER barrierInfo{};
	barrierInfo.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierInfo.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrierInfo.Transition.pResource = resource;
	barrierInfo.Transition.StateBefore = GetGpuImageLayoutDx12(previousLayout);
	barrierInfo.Transition.StateAfter = GetGpuImageLayoutDx12(nextLayout);
	barrierInfo.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrierInfo);

	image->SetLayout(nextLayout);
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
	copySource.PlacedFootprint.Footprint.Height = dest->GetPhysicalSize().Y;
	copySource.PlacedFootprint.Footprint.RowPitch = dest->GetPhysicalSize().X * GetFormatNumberOfBytes(dest->GetFormat());
	copySource.PlacedFootprint.Offset = offset;

	commandList->CopyTextureRegion(&copyDest, 0, 0, 0, &copySource, nullptr);
}

void CommandListDx12::CopyImageToImage(const GpuImage* source, GpuImage* destination, TSize numLayers, TSize srcStartLayer, TSize dstStartLayer, TSize srcMipLevel, TSize dstMipLevel, Vector2ui copySize) {
	OSK_ASSERT(false, "No implementado");
}

void CommandListDx12::BeginGraphicsRenderpass(DynamicArray<RenderPassImageInfo> colorImages, RenderPassImageInfo depthImage, const Color& color) {
	DynamicArray<D3D12_CPU_DESCRIPTOR_HANDLE> colorAttachments{};
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDesc{};

	if (currentRenderpassType== RenderpassType::FINAL) {
		const SwapchainDx12* swapchain = Engine::GetRenderer()->_GetSwapchain()->As<SwapchainDx12>();

		colorAttachments.Insert(swapchain->GetRenderTargetMemory()->GetCPUDescriptorHandleForHeapStart());
		colorAttachments[0].ptr += ((SIZE_T)swapchain->GetCurrentFrameIndex()) *
			Engine::GetRenderer()->As<RendererDx12>()->GetGpu()->As<GpuDx12>()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		depthStencilDesc = swapchain->GetDepthStencilMemory()->GetCPUDescriptorHandleForHeapStart();
		depthStencilDesc.ptr += ((SIZE_T)swapchain->GetCurrentFrameIndex()) *
			Engine::GetRenderer()->As<RendererDx12>()->GetGpu()->As<GpuDx12>()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}
	else {
		for (const auto& img : colorImages)
			colorAttachments.Insert(img.targetImage->As<GpuImageDx12>()->GetColorUsageDescriptorHandle());

		depthStencilDesc = depthImage.targetImage->As<GpuImageDx12>()->GetDepthUsageDescriptorHandle();
	}

	for (const auto& img : colorImages) {
		const GpuBarrierInfo prev(GpuBarrierStage::DEFAULT, GpuBarrierAccessStage::DEFAULT);
		const GpuBarrierInfo next(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE);
		const GpuImageBarrierInfo imgInfo = {
			.baseLayer = img.arrayLevel,
			.numLayers = 1,
			.baseMipLevel = 0,
			.numMipLevels = 1,
		};

		SetGpuImageBarrier(img.targetImage, GpuImageLayout::UNDEFINED, GpuImageLayout::COLOR_ATTACHMENT, prev, next, imgInfo);
	}

	const FLOAT clearValue[] = { color.Red, color.Green, color.Blue, color.Alpha };
	
	for (auto colorAttachment : colorAttachments)
		commandList->ClearRenderTargetView(colorAttachment, clearValue, 0, nullptr);
	commandList->ClearDepthStencilView(depthStencilDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	commandList->OMSetRenderTargets(colorAttachments.GetSize(), colorAttachments.GetData(), FALSE, &depthStencilDesc);

	currentColorImages = std::move(colorImages);
	currentDepthImage = depthImage;
}

void CommandListDx12::EndGraphicsRenderpass() {
	GpuImageLayout finalLayout = GpuImageLayout::SHADER_READ_ONLY;
	if (currentRenderpassType == RenderpassType::FINAL)
		finalLayout = GpuImageLayout::PRESENT;

	for (const auto& img : currentColorImages) {
		GpuBarrierInfo prevBarrier{};
		prevBarrier.stage = GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT;
		prevBarrier.accessStage = GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE;

		GpuBarrierInfo nextBarrier{};
		nextBarrier.stage = GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT;
		nextBarrier.accessStage = GpuBarrierAccessStage::SHADER_READ;

		GpuImageBarrierInfo imgBarrier{};
		imgBarrier.baseLayer = img.arrayLevel;
		imgBarrier.numLayers = 1;
		imgBarrier.baseMipLevel = 0;
		imgBarrier.numMipLevels = ALL_MIP_LEVELS;

		SetGpuImageBarrier(img.targetImage, GpuImageLayout::COLOR_ATTACHMENT, finalLayout, prevBarrier, nextBarrier, imgBarrier);
	}

	if (currentRenderpassType != RenderpassType::INTERMEDIATE)
		currentRenderpassType = RenderpassType::INTERMEDIATE;
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

void CommandListDx12::BindMaterial(Material* material) {
	currentMaterial = material;

	DynamicArray<Format> colorFormats;
	for (const auto& colorImg : currentColorImages)
		colorFormats.Insert(colorImg.targetImage->GetFormat());

	currentPipeline.graphics = material->GetGraphicsPipeline(colorFormats);
	
	commandList->SetGraphicsRootSignature(currentPipeline.graphics->As<GraphicsPipelineDx12>()->GetLayout());
	commandList->SetPipelineState(currentPipeline.graphics->As<GraphicsPipelineDx12>()->GetPipelineState());

	commandList->IASetPrimitiveTopology(currentPipeline.graphics->As<GraphicsPipelineDx12>()->GetTopologyType());
}

void CommandListDx12::BindVertexBuffer(const IGpuVertexBuffer* buffer) {
	D3D12_VERTEX_BUFFER_VIEW views[] = {
		buffer->As<GpuVertexBufferDx12>()->GetView()
	};

	commandList->IASetVertexBuffers(0, 1, views);
}

void CommandListDx12::BindIndexBuffer(const IGpuIndexBuffer* buffer) {
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

void CommandListDx12::TraceRays(TSize raygenEntry, TSize closestHitEntry, TSize missEntry, const Vector2ui& resolution) {
	OSK_ASSERT(false, "No implementado.");
}

void CommandListDx12::DispatchCompute(const Vector3ui& groupCount) {
	OSK_ASSERT(false, "No implementado.");
}

void CommandListDx12::BindComputePipeline(const IComputePipeline& pipeline) {
	OSK_ASSERT(false, "No implementado.");
}

void CommandListDx12::SetViewport(const Viewport& vp) {
	D3D12_VIEWPORT viewport{};

	viewport.TopLeftX = (FLOAT)vp.rectangle.GetRectanglePosition().X;
	viewport.TopLeftY = (FLOAT)vp.rectangle.GetRectanglePosition().Y;
	viewport.Width = (FLOAT)vp.rectangle.GetRectangleSize().X;
	viewport.Height = (FLOAT)vp.rectangle.GetRectangleSize().Y;

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

void CommandListDx12::SetDebugName(const std::string& name) {
	commandList->SetName(StringToWideString(name).c_str());
}
