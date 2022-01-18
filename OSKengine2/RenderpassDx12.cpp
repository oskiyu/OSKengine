#include "RenderpassDx12.h"

#include "GpuImageDx12.h"
#include "OSKengine.h"
#include "RendererDx12.h"
#include "GpuMemoryAllocatorDx12.h"
#include "Format.h"
#include "GpuMemoryTypes.h"

using namespace OSK;

RenderpassDx12::RenderpassDx12(RenderpassType type)
	: IRenderpass(type) {

}

RenderpassDx12::~RenderpassDx12() {

}

void RenderpassDx12::SetImage(GpuImage* image) {
	const auto size = image->GetSize();

	images[0] = image;
	images[1] = nullptr;

	colorImgs[0] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
		size.X, size.Y, image->GetFormat(),
		GpuImageUsage::COLOR, GpuSharedMemoryType::GPU_ONLY).GetPointer();

	depthImgs[0] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
		size.X, size.Y, Format::D24S8_SFLOAT_SUINT,
		GpuImageUsage::DEPTH_STENCIL, GpuSharedMemoryType::GPU_ONLY).GetPointer();
}

void RenderpassDx12::SetImages(GpuImage* image0, GpuImage* image1, GpuImage* image2) {
	const auto size = image0->GetSize();

	images[0] = image0;
	images[1] = image1;
	images[2] = image2;

	for (TSize i = 0; i < 3; i++) {
		colorImgs[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
			size.X, size.Y, image0->GetFormat(),
			GpuImageUsage::COLOR, GpuSharedMemoryType::GPU_ONLY).GetPointer();

		depthImgs[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
			size.X, size.Y, Format::D24S8_SFLOAT_SUINT,
			GpuImageUsage::DEPTH_STENCIL, GpuSharedMemoryType::GPU_ONLY).GetPointer();
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
