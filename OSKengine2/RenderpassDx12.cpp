#include "RenderpassDx12.h"

#include "GpuImageDx12.h"
#include "OSKengine.h"
#include "RendererDx12.h"
#include "GpuMemoryAllocatorDx12.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageUsage.h"
#include "FormatDx12.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuDx12.h"
#include "RenderpassType.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

RenderpassDx12::RenderpassDx12(RenderpassType type)
	: IRenderpass(type) {

}

RenderpassDx12::~RenderpassDx12() {

}

void RenderpassDx12::SetImages(GpuImage* image0, GpuImage* image1, GpuImage* image2) {
	const auto size = image0->GetSize();

	images[0] = image0;
	images[1] = image1;
	images[2] = image2;

	const GpuDx12* device = Engine::GetRenderer()->GetGpu()->As<GpuDx12>();

	for (TSize i = 0; i < 3; i++) {
		colorImgs[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
			size, images[i]->GetDimension(), images[i]->GetNumLayers(), images[i]->GetFormat(),
			GpuImageUsage::COLOR, GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();

		depthImgs[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
			size, images[i]->GetDimension(), images[i]->GetNumLayers(), Format::D24S8_SFLOAT_SUINT,
			GpuImageUsage::DEPTH_STENCIL, GpuSharedMemoryType::GPU_ONLY, 1).GetPointer();

			
		if (type == RenderpassType::INTERMEDIATE) {
			const GpuImageDx12* dxImage = images[i]->As<GpuImageDx12>();

			// Render target.
			D3D12_RENDER_TARGET_VIEW_DESC renderTargetDesc{};
			renderTargetDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			renderTargetDesc.Format = GetFormatDx12(images[i]->GetFormat());
			renderTargetDesc.Texture2D.MipSlice = 0;
			renderTargetDesc.Texture2D.PlaneSlice = 0;

			colorDescs[i] = dxImage->GetRenderTargetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

			device->GetDevice()->CreateRenderTargetView(dxImage->GetResource(), &renderTargetDesc, colorDescs[i]);

			// Depth/stencil.
			D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
			depthStencilViewDesc.Format = GetFormatDx12(depthImgs[i]->GetFormat());
			depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

			depthDescs[i] = depthImgs[i]->As<GpuImageDx12>()->GetDepthDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

			device->As<GpuDx12>()->GetDevice()->CreateDepthStencilView(depthImgs[i]->As<GpuImageDx12>()->GetResource(), &depthStencilViewDesc, depthDescs[i]);
		}
	}
}

void RenderpassDx12::SetSwapchain(SwapchainDx12* swapchain) {
	this->swapchain = swapchain;
}

SwapchainDx12* RenderpassDx12::GetSwapchain() const {
	return swapchain;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderpassDx12::GetRenderTargetDescpriptor(TSize index) const {
	return colorDescs[index];
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderpassDx12::GetDepthStencilDescpriptor(TSize index) const {
	return depthDescs[index];
}
