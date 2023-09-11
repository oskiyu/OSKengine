#include "CommandListDx12.h"

#include "GraphicsPipelineDx12.h"
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
#include "GpuBuffer.h"
#include "Viewport.h"
#include "MaterialSlotDx12.h"
#include "Material.h"
#include "FormatDx12.h"
#include "Format.h"
#include "MaterialLayout.h"
#include "Math.h"
#include "WindowsUtils.h"
#include "GpuImageViewDx12.h"
#include "PipelineLayoutDx12.h"
#include "IGpuImageView.h"
#include "ComputePipelineDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

USize64 Multiplo256(USize64 original) {
	USize64 output = original / 256;
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
void CommandListDx12::ClearImage(GpuImage* image, const GpuImageRange& range, const Color& color) {
	// commandList->ClearRenderTargetView();
}
void CommandListDx12::SetGpuImageBarrier(GpuImage* image, GpuImageLayout previousLayout, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageRange& prevImageInfo) {
	ID3D12Resource* resource = image->As<GpuImageDx12>()->GetResource();
	
	D3D12_RESOURCE_BARRIER barrierInfo{};
	barrierInfo.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierInfo.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrierInfo.Transition.pResource = resource;
	barrierInfo.Transition.StateBefore = GetGpuImageLayoutDx12(previousLayout);
	barrierInfo.Transition.StateAfter = GetGpuImageLayoutDx12(nextLayout);
	barrierInfo.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrierInfo);

	image->_SetLayout(
		prevImageInfo.baseLayer,
		prevImageInfo.numLayers,
		prevImageInfo.baseMipLevel,
		prevImageInfo.numLayers,
		nextLayout);
}

void CommandListDx12::CopyBufferToImage(const GpuBuffer& source, GpuImage* dest, UIndex32 layer, USize64 offset) {
	D3D12_TEXTURE_COPY_LOCATION copyDest{};
	copyDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	copyDest.SubresourceIndex = layer;
	copyDest.pResource = dest->GetBuffer()->As<GpuMemorySubblockDx12>()->GetResource();

	D3D12_TEXTURE_COPY_LOCATION copySource{};
	copySource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	copySource.pResource = source.GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource();
	copySource.PlacedFootprint.Footprint.Depth = 1;
	copySource.PlacedFootprint.Footprint.Format = GetFormatDx12(dest->GetFormat());
	copySource.PlacedFootprint.Footprint.Width = dest->GetSize3D().x;
	copySource.PlacedFootprint.Footprint.Height = dest->GetPhysicalSize().y;
	copySource.PlacedFootprint.Footprint.RowPitch = dest->GetPhysicalSize().x * GetFormatNumberOfBytes(dest->GetFormat());
	copySource.PlacedFootprint.Offset = offset;

	commandList->CopyTextureRegion(&copyDest, 0, 0, 0, &copySource, nullptr);
}

void CommandListDx12::RawCopyImageToImage(const GpuImage& source, GpuImage* destination, const CopyImageInfo& copyInfo) {
	D3D12_TEXTURE_COPY_LOCATION copyDest{};
	copyDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	copyDest.SubresourceIndex = copyInfo.destinationArrayLevel;
	copyDest.pResource = destination->GetBuffer()->As<GpuMemorySubblockDx12>()->GetResource();

	D3D12_TEXTURE_COPY_LOCATION copySource{};
	copySource.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	copySource.SubresourceIndex = copyInfo.sourceArrayLevel;
	copySource.pResource = source.GetBuffer()->As<GpuMemorySubblockDx12>()->GetResource();
	
	D3D12_BOX copyRegion{};
	copyRegion.left = 0;
	copyRegion.right = copyInfo.copySize.x;
	copyRegion.top = 0;
	copyRegion.bottom = copyInfo.copySize.y;
	copyRegion.back = 1;
	copyRegion.front = 0;

	commandList->CopyTextureRegion(&copyDest, copyInfo.destinationOffset.x, copyInfo.destinationOffset.y, copyInfo.destinationOffset.Z, &copySource, &copyRegion);
}

void CommandListDx12::CopyImageToImage(const GpuImage& source, GpuImage* destination, const CopyImageInfo& copyInfo, GpuImageFilteringType filter) {
	OSK_ASSERT(false, NotImplementedException());
}

void CommandListDx12::CopyBufferToBuffer(const GpuBuffer& source, GpuBuffer* dest, USize64 size, USize64 sourceOffset, USize64 destOffset) {
	commandList->CopyBufferRegion(
		dest->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource(), destOffset,
		source.GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource(), sourceOffset, size);
}

void CommandListDx12::BeginGraphicsRenderpass(DynamicArray<RenderPassImageInfo> colorImages, RenderPassImageInfo depthImage, const Color& color, bool autoSync) {
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
		GpuImageViewConfig colorView = GpuImageViewConfig::CreateTarget_Color();
		GpuImageViewConfig depthView = GpuImageViewConfig::CreateTarget_DepthStencil();
		depthView.baseArrayLevel = depthImage.arrayLevel;

		for (const auto& [image, layer] : colorImages) {
			colorView.baseArrayLevel = layer;
			colorAttachments.Insert(image->GetView(colorView)
				->As<GpuImageViewDx12>()->GetCpuAddress());
		}

		depthStencilDesc = depthImage.targetImage->GetView(depthView)
			->As<GpuImageViewDx12>()->GetCpuAddress();
	}

	for (const auto& img : colorImages) {
		const GpuBarrierInfo prev(GpuCommandStage::NONE, GpuAccessStage::NONE);
		const GpuBarrierInfo next(GpuCommandStage::COLOR_ATTACHMENT_OUTPUT, GpuAccessStage::COLOR_ATTACHMENT_WRITE);
		const GpuImageRange imgInfo = {
			.baseLayer = img.arrayLevel,
			.numLayers = 1,
			.baseMipLevel = 0,
			.numMipLevels = 1,
		};

		if (autoSync)
		SetGpuImageBarrier(img.targetImage, GpuImageLayout::UNDEFINED, GpuImageLayout::COLOR_ATTACHMENT, prev, next, imgInfo);
	}

	const FLOAT clearValue[] = { color.red, color.green, color.blue, color.alpha };
	
	for (auto colorAttachment : colorAttachments)
		commandList->ClearRenderTargetView(colorAttachment, clearValue, 0, nullptr);
	commandList->ClearDepthStencilView(depthStencilDesc, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	commandList->OMSetRenderTargets(static_cast<UINT>(colorAttachments.GetSize()), colorAttachments.GetData(), FALSE, &depthStencilDesc);

	currentColorImages = std::move(colorImages);
	currentDepthImage = depthImage;
}

void CommandListDx12::EndGraphicsRenderpass(bool autoSync) {
	GpuImageLayout finalLayout = GpuImageLayout::SAMPLED;
	if (currentRenderpassType == RenderpassType::FINAL)
		finalLayout = GpuImageLayout::PRESENT;

	for (const auto& img : currentColorImages) {
		GpuBarrierInfo prevBarrier{};
		prevBarrier.stage = GpuCommandStage::COLOR_ATTACHMENT_OUTPUT;
		prevBarrier.accessStage = GpuAccessStage::COLOR_ATTACHMENT_WRITE;

		GpuBarrierInfo nextBarrier{};
		nextBarrier.stage = GpuCommandStage::COLOR_ATTACHMENT_OUTPUT;
		nextBarrier.accessStage = GpuAccessStage::SHADER_READ;

		GpuImageRange imgBarrier{};
		imgBarrier.baseLayer = img.arrayLevel;
		imgBarrier.numLayers = 1;
		imgBarrier.baseMipLevel = 0;
		imgBarrier.numMipLevels = ALL_MIP_LEVELS;

		if (autoSync)
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

void CommandListDx12::BindGraphicsPipeline(const IGraphicsPipeline& graphicsPipeline) {
	commandList->SetGraphicsRootSignature(graphicsPipeline.GetLayout()->As<PipelineLayoutDx12>()->GetSignature());
	commandList->SetPipelineState(graphicsPipeline.As<GraphicsPipelineDx12>()->GetPipeline());

	commandList->IASetPrimitiveTopology(currentPipeline.graphics->As<GraphicsPipelineDx12>()->GetTopologyType());
}

void CommandListDx12::BindComputePipeline(const IComputePipeline& computePipeline) {
	commandList->SetComputeRootSignature(computePipeline.GetLayout()->As<PipelineLayoutDx12>()->GetSignature());
	commandList->SetPipelineState(computePipeline.As<ComputePipelineDx12>()->GetPipeline());
}

void CommandListDx12::BindRayTracingPipeline(const IRaytracingPipeline& computePipeline) {
	OSK_ASSERT(false, NotImplementedException());
}

void CommandListDx12::BindVertexBufferRange(const GpuBuffer& buffer, const VertexBufferView& view) {
	const D3D12_VERTEX_BUFFER_VIEW dxview {
		.BufferLocation = buffer.GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress() + view.offsetInBytes,
		.SizeInBytes = static_cast<UINT>(buffer.GetSize() - view.offsetInBytes),
		.StrideInBytes = view.vertexInfo.GetSize()
	};

	commandList->IASetVertexBuffers(0, 1, &dxview);
}

void CommandListDx12::BindIndexBufferRange(const GpuBuffer& buffer, const IndexBufferView& view) {
	const D3D12_INDEX_BUFFER_VIEW dxview = {
		.BufferLocation = buffer.GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress() + view.offsetInBytes,
		.SizeInBytes = static_cast<UINT>(buffer.GetSize() - view.offsetInBytes),
		.Format = GetIndexFormat(view.type)
	};
	commandList->IASetIndexBuffer(&dxview);
}

void CommandListDx12::BindMaterialSlot(const IMaterialSlot& slot) {
	for (const auto& [index, buffer] : slot.As<MaterialSlotDx12>()->GetUniformBuffers()) {
		if (buffer != nullptr) {
			switch (currentMaterial->GetMaterialType())	{
			case MaterialType::GRAPHICS:
				commandList->SetGraphicsRootConstantBufferView(index,
					buffer->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress());
				break;
			case MaterialType::COMPUTE:
				commandList->SetComputeRootConstantBufferView(index,
					buffer->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress());
				break;
			}
		}
	}

	for (const auto& [index, buffer] : slot.As<MaterialSlotDx12>()->GetStorageBuffers()) {
		if (buffer != nullptr) {
			switch (currentMaterial->GetMaterialType()) {
			case MaterialType::GRAPHICS:
				commandList->SetGraphicsRootUnorderedAccessView(index,
					buffer->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress());
				break;
			case MaterialType::COMPUTE:
				commandList->SetComputeRootUnorderedAccessView(index,
					buffer->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress());
				break;
			}
		}
	}

	for (const auto& [index, image] : slot.As<MaterialSlotDx12>()->GetGpuImages()) {
		if (image != nullptr) {
			const auto viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
			const GpuImageViewDx12* view = image->GetView(viewConfig)->As<GpuImageViewDx12>();

			ID3D12DescriptorHeap* heaps[] = { view->GetDescriptorHeap() };

			commandList->SetDescriptorHeaps(1, heaps);
			
			switch (currentMaterial->GetMaterialType()) {
			case MaterialType::GRAPHICS:
				commandList->SetGraphicsRootDescriptorTable(index, view->GetGpuAddress());
				break;
			case MaterialType::COMPUTE:
				commandList->SetComputeRootDescriptorTable(index, view->GetGpuAddress());
				break;
			}
		}
	}

	for (const auto& [index, image] : slot.As<MaterialSlotDx12>()->GetStorageBuffers()) {
		if (image != nullptr) {
			switch (currentMaterial->GetMaterialType()) {
			case MaterialType::GRAPHICS:
				commandList->SetGraphicsRootUnorderedAccessView(index,
					image->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress());
				break;
			case MaterialType::COMPUTE:
				commandList->SetComputeRootUnorderedAccessView(index,
					image->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress());
				break;
			}
		}
	}
}

void CommandListDx12::PushMaterialConstants(const std::string& pushConstName, const void* data, USize32 size, USize32 offset) {
	auto& pushConst = currentMaterial->GetLayout()->GetPushConstant(pushConstName);

	USize32 nSize = size / 4;
	if (size % 4)
		nSize++;

	commandList->SetGraphicsRoot32BitConstants(pushConst.hlslBindingIndex, nSize, data, pushConst.offset + offset);
}

void CommandListDx12::BindUniformBufferDx12(UIndex32 index, const GpuBuffer* buffer) {
	commandList->SetGraphicsRootConstantBufferView(index,
		buffer->GetMemorySubblock()->As<GpuMemorySubblockDx12>()->GetResource()->GetGPUVirtualAddress());
}

void CommandListDx12::DrawSingleInstance(USize32 numIndices) {
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

void CommandListDx12::DrawSingleMesh(UIndex32 firstIndex, USize32 numIndices) {
	commandList->DrawIndexedInstanced(numIndices, 1, firstIndex, 0, 0);
}

void CommandListDx12::TraceRays(UIndex32 raygenEntry, UIndex32 closestHitEntry, UIndex32 missEntry, const Vector2ui& resolution) {
	OSK_ASSERT(false, NotImplementedException());
}

void CommandListDx12::DispatchCompute(const Vector3ui& groupCount) {
	OSK_ASSERT(false, NotImplementedException());
}

void CommandListDx12::SetViewport(const Viewport& vp) {
	D3D12_VIEWPORT viewport{};

	viewport.TopLeftX = (FLOAT)vp.rectangle.GetRectanglePosition().x;
	viewport.TopLeftY = (FLOAT)vp.rectangle.GetRectanglePosition().y;
	viewport.Width = (FLOAT)vp.rectangle.GetRectangleSize().x;
	viewport.Height = (FLOAT)vp.rectangle.GetRectangleSize().y;

	viewport.MinDepth = vp.minDepth;
	viewport.MaxDepth = vp.maxDepth;

	commandList->RSSetViewports(1, &viewport);
}

void CommandListDx12::SetScissor(const Vector4ui& scissor) {
	D3D12_RECT scissorRect{};

	scissorRect.left = scissor.GetRectanglePosition().x;
	scissorRect.top = scissor.GetRectanglePosition().y;
	scissorRect.right = scissor.GetRectangleSize().x;
	scissorRect.bottom = scissor.GetRectangleSize().y;

	commandList->RSSetScissorRects(1, &scissorRect);
}

void CommandListDx12::SetDebugName(const std::string& name) {
	commandList->SetName(StringToWideString(name).c_str());
}

void CommandListDx12::AddDebugMarker(const std::string& mark, const Color& color) {}
void CommandListDx12::StartDebugSection(const std::string& mark, const Color& color) {}
void CommandListDx12::EndDebugSection() {}


DXGI_FORMAT CommandListDx12::GetIndexFormat(IndexType type) {
	switch (type)
	{
	case OSK::GRAPHICS::IndexType::U8:
		return DXGI_FORMAT_R8_UINT;
	case OSK::GRAPHICS::IndexType::U16:
		return DXGI_FORMAT_R16_UINT;
	case OSK::GRAPHICS::IndexType::U32:
		return DXGI_FORMAT_R32_UINT;
	default:
		OSK_ASSERT(false, NotImplementedException());
		return DXGI_FORMAT_UNKNOWN;
	}
}
