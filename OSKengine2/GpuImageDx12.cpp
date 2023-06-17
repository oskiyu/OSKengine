#include "GpuImageDx12.h"

#include "WindowsUtils.h"

#include "OSKengine.h"
#include "RendererDx12.h"
#include "GpuDx12.h"
#include "FormatDx12.h"
#include "GpuImageViewDx12.h"
#include "GpuMemoryAllocatorDx12.h"

#include "RendererExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageDx12::GpuImageDx12(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, USize32 numLayers, Format format, USize32 numSamples, GpuImageSamplerDesc samplerDesc)
	: GpuImage(size, dimension, usage, numLayers, format, numSamples, samplerDesc) {

}

void GpuImageDx12::FillResourceDesc() {
	resourceDesc.Width = GetSize2D().x;
	resourceDesc.Height = GetSize2D().y;
	resourceDesc.DepthOrArraySize = GetNumLayers() == 1 ? GetPhysicalSize().Z : GetNumLayers();
	resourceDesc.Dimension = (D3D12_RESOURCE_DIMENSION)((USize64)GetDimension() + 1);
	resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.MipLevels = GetMipLevels();
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Format = GetFormatDx12(GetFormat());

	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
	if (EFTraits::HasFlag(GetUsage(), GpuImageUsage::COLOR))
		flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (EFTraits::HasFlag(GetUsage(), GpuImageUsage::STENCIL))
		flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	if (EFTraits::HasFlag(GetUsage(), GpuImageUsage::DEPTH))
		flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	if (EFTraits::HasFlag(GetUsage(), GpuImageUsage::COMPUTE))
		flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	resourceDesc.Flags = flags;
}

const D3D12_RESOURCE_DESC& GpuImageDx12::GetResourceDesc() {
	return resourceDesc;
}

void GpuImageDx12::_SetResource(ComPtr<ID3D12Resource> resource) {
	this->resource = resource;
}

void GpuImageDx12::CreateResource(ID3D12Heap* memory, USize64 memoryOffset) {
	Engine::GetRenderer()->GetGpu()->As<GpuDx12>()->GetDevice()->CreatePlacedResource(memory, memoryOffset, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
}

ID3D12Resource* GpuImageDx12::GetResource() const {
	return resource.Get();
}

void GpuImageDx12::SetDebugName(const std::string& name) {
	auto str = StringToWideString(name);
	resource->SetName(str.c_str());
}

OwnedPtr<IGpuImageView> GpuImageDx12::CreateView(const GpuImageViewConfig& config) const {
	switch (config.usage) {
	case OSK::GRAPHICS::ViewUsage::SAMPLED: {
		const auto descriptor = Engine::GetRenderer()->GetAllocator()->As<GpuMemoryAllocatorDx12>()->GetDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
		if (config.channel == SampledChannel::COLOR) {
			D3D12_SHADER_RESOURCE_VIEW_DESC resourceViewDesc{};
			resourceViewDesc.Format = GetFormatDx12(GetFormat());
			resourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			if (config.arrayType == SampledArrayType::SINGLE_LAYER) {
				resourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				resourceViewDesc.Texture2D.MipLevels = GetMipLevels();
				resourceViewDesc.Texture2D.MostDetailedMip = 0;
				resourceViewDesc.Texture2D.PlaneSlice = config.baseArrayLevel;
			}
			else if (config.arrayType == SampledArrayType::ARRAY) {
				resourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

				resourceViewDesc.Texture2DArray.MostDetailedMip = 0;
				resourceViewDesc.Texture2DArray.MipLevels = GetMipLevels();

				resourceViewDesc.Texture2DArray.FirstArraySlice = config.baseArrayLevel;
				resourceViewDesc.Texture2DArray.ArraySize = config.arrayLevelCount;
				resourceViewDesc.Texture2DArray.PlaneSlice = 0;
			}

			Engine::GetRenderer()->GetGpu()->As<GpuDx12>()->GetDevice()
				->CreateShaderResourceView(GetResource(), &resourceViewDesc, descriptor.cpuHandle);
		}
		else if (config.channel == SampledChannel::DEPTH) {
			D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
			desc.Format = GetFormatDx12(GetFormat());
			desc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;

			if (config.arrayType == SampledArrayType::SINGLE_LAYER) {
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = config.baseArrayLevel;
			}
			else if (config.arrayType == SampledArrayType::ARRAY) {
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				desc.Texture2DArray.MipSlice = 0;
				desc.Texture2DArray.FirstArraySlice = config.baseArrayLevel;
				desc.Texture2DArray.ArraySize = config.arrayLevelCount;
			}

			Engine::GetRenderer()->GetGpu()->As<GpuDx12>()->GetDevice()
				->CreateDepthStencilView(GetResource(), &desc, descriptor.cpuHandle);
		}
		else if (config.channel == SampledChannel::STENCIL) {
			D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
			desc.Format = GetFormatDx12(GetFormat());
			desc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;

			if (config.arrayType == SampledArrayType::SINGLE_LAYER) {
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = config.baseArrayLevel;
			}
			else if (config.arrayType == SampledArrayType::ARRAY) {
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				desc.Texture2DArray.MipSlice = 0;
				desc.Texture2DArray.FirstArraySlice = config.baseArrayLevel;
				desc.Texture2DArray.ArraySize = config.arrayLevelCount;
			}

			Engine::GetRenderer()->GetGpu()->As<GpuDx12>()->GetDevice()
				->CreateDepthStencilView(GetResource(), &desc, descriptor.cpuHandle);
		}

		return new GpuImageViewDx12(this, descriptor, config);
	}
		break;

	case OSK::GRAPHICS::ViewUsage::COLOR_TARGET: {
		const auto descriptor = Engine::GetRenderer()->GetAllocator()->As<GpuMemoryAllocatorDx12>()->GetDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_RENDER_TARGET_VIEW_DESC resourceViewDesc{};
		resourceViewDesc.Format = GetFormatDx12(GetFormat());

		if (config.arrayType == SampledArrayType::SINGLE_LAYER) {
			resourceViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

			resourceViewDesc.Texture2D.MipSlice = 0;
			resourceViewDesc.Texture2D.PlaneSlice = config.baseArrayLevel;
		}
		else if (config.arrayType == SampledArrayType::ARRAY) {
			D3D12_RENDER_TARGET_VIEW_DESC resourceViewDesc{};
			resourceViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;

			resourceViewDesc.Texture2DArray.MipSlice = 0;
			resourceViewDesc.Texture2DArray.PlaneSlice = 0;
			resourceViewDesc.Texture2DArray.FirstArraySlice = config.baseArrayLevel;
			resourceViewDesc.Texture2DArray.ArraySize = config.arrayLevelCount;
		}

		Engine::GetRenderer()->GetGpu()->As<GpuDx12>()->GetDevice()
			->CreateRenderTargetView(GetResource(), &resourceViewDesc, descriptor.cpuHandle);

		return new GpuImageViewDx12(this, descriptor, config);
	}
		break;

	case OSK::GRAPHICS::ViewUsage::DEPTH_STENCIL_TARGET: {
		const auto descriptor = Engine::GetRenderer()->GetAllocator()->As<GpuMemoryAllocatorDx12>()->GetDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		
		D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
		desc.Format = GetFormatDx12(GetFormat());
		desc.Flags = D3D12_DSV_FLAG_NONE;

		if (config.arrayType == SampledArrayType::SINGLE_LAYER) {
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;
		}
		else if (config.arrayType == SampledArrayType::ARRAY) {
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MipSlice = 0;
			desc.Texture2DArray.FirstArraySlice = config.baseArrayLevel;
			desc.Texture2DArray.ArraySize = config.arrayLevelCount;
		}

		Engine::GetRenderer()->GetGpu()->As<GpuDx12>()->GetDevice()
			->CreateDepthStencilView(GetResource(), &desc, descriptor.cpuHandle);

		return new GpuImageViewDx12(this, descriptor, config);
	}
		break;

	case OSK::GRAPHICS::ViewUsage::STORAGE: {
		const auto descriptor = Engine::GetRenderer()->GetAllocator()->As<GpuMemoryAllocatorDx12>()->GetDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
		desc.Format = GetFormatDx12(GetFormat());

		if (config.arrayType == SampledArrayType::SINGLE_LAYER) {
			desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;
			desc.Texture2D.PlaneSlice = config.baseArrayLevel;
		}
		else if (config.arrayType == SampledArrayType::ARRAY) {
			desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MipSlice = 0;
			desc.Texture2DArray.PlaneSlice = 0;
			desc.Texture2DArray.FirstArraySlice = config.baseArrayLevel;
			desc.Texture2DArray.ArraySize = config.arrayLevelCount;
		}

		Engine::GetRenderer()->GetGpu()->As<GpuDx12>()->GetDevice()
			->CreateUnorderedAccessView(GetResource(), nullptr, &desc, descriptor.cpuHandle);

		return new GpuImageViewDx12(this, descriptor, config);
	}
		break;
	}
	
	OSK_ASSERT(false, ImageViewCreationException(0));
	return nullptr;
}
