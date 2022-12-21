#include "PipelineLayoutDx12.h"

#include "MaterialLayout.h"
#include "ShaderBindingType.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuDx12.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

D3D12_SHADER_VISIBILITY GetShaderStageDx12(ShaderStage stage) {
	if (OSK::EFTraits::HasFlag(stage, ShaderStage::VERTEX)
		&& OSK::EFTraits::HasFlag(stage, ShaderStage::FRAGMENT))
		return D3D12_SHADER_VISIBILITY_ALL;

	switch (stage) {
	case ShaderStage::VERTEX:
		return D3D12_SHADER_VISIBILITY_VERTEX;
	case ShaderStage::FRAGMENT:
		return D3D12_SHADER_VISIBILITY_PIXEL;
	case ShaderStage::TESSELATION_CONTROL:
		return D3D12_SHADER_VISIBILITY_ALL;
	case ShaderStage::TESSELATION_EVALUATION:
		return D3D12_SHADER_VISIBILITY_ALL;
	default:
		return D3D12_SHADER_VISIBILITY_ALL;
	}
}

D3D12_ROOT_PARAMETER_TYPE GetParamTypeDx12(ShaderBindingType type) {
	switch (type) {
	case ShaderBindingType::UNIFORM_BUFFER:
		return D3D12_ROOT_PARAMETER_TYPE_CBV;
	case ShaderBindingType::STORAGE_BUFFER:
	case ShaderBindingType::STORAGE_IMAGE:
		return D3D12_ROOT_PARAMETER_TYPE_UAV;
	case ShaderBindingType::TEXTURE:
		//break;
	default:
		return D3D12_ROOT_PARAMETER_TYPE_CBV;
	}
}


PipelineLayoutDx12::PipelineLayoutDx12(const MaterialLayout* layout)
	: IPipelineLayout(layout) {

	// Owned.
	DynamicArray<UniquePtr<D3D12_DESCRIPTOR_RANGE>> descriptorRanges;
	// Owned.
	DynamicArray<D3D12_STATIC_SAMPLER_DESC> staticSamplers;

	DynamicArray<D3D12_ROOT_PARAMETER> nativeParams;
	for (auto& [setName, slot] : layout->GetAllSlots()) {
		for (auto& [name, binding] : slot.bindings) {
			if (binding.type == ShaderBindingType::UNIFORM_BUFFER) {
				D3D12_ROOT_PARAMETER param{};
				param.ShaderVisibility = GetShaderStageDx12(slot.stage);
				param.ParameterType = GetParamTypeDx12(binding.type);
				param.Descriptor.RegisterSpace = 0;
				param.Descriptor.ShaderRegister = binding.hlslIndex;

				nativeParams.Insert(param);
			}
			else if (binding.type == ShaderBindingType::TEXTURE
					|| binding.type == ShaderBindingType::CUBEMAP) {
				OwnedPtr<D3D12_DESCRIPTOR_RANGE> textureDescriptorRange = new D3D12_DESCRIPTOR_RANGE({});
				textureDescriptorRange[0].BaseShaderRegister = binding.hlslIndex;
				textureDescriptorRange[0].NumDescriptors = 1;
				textureDescriptorRange[0].OffsetInDescriptorsFromTableStart = 0;
				textureDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				textureDescriptorRange[0].RegisterSpace = 0;

				D3D12_STATIC_SAMPLER_DESC sampler{};
				sampler.RegisterSpace = 0;
				sampler.ShaderRegister = binding.hlslIndex;
				sampler.ShaderVisibility = GetShaderStageDx12(slot.stage);

				sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
				sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
				sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
				sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
				sampler.MipLODBias = 0;
				sampler.MaxAnisotropy = 16;
				sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
				sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
				sampler.MinLOD = 0.0f;
				sampler.MaxLOD = D3D12_FLOAT32_MAX;

				D3D12_ROOT_PARAMETER param{};
				param.ShaderVisibility = GetShaderStageDx12(slot.stage);
				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = textureDescriptorRange.GetPointer();

				descriptorRanges.Insert(textureDescriptorRange.GetPointer());
				staticSamplers.Insert(sampler);

				nativeParams.Insert(param);
			}
			else if (binding.type == ShaderBindingType::STORAGE_BUFFER) {
				
			}
			else if(binding.type == ShaderBindingType::STORAGE_IMAGE) {
				OwnedPtr<D3D12_DESCRIPTOR_RANGE> textureDescriptorRange = new D3D12_DESCRIPTOR_RANGE({});
				textureDescriptorRange[0].BaseShaderRegister = binding.hlslDescriptorIndex;
				textureDescriptorRange[0].NumDescriptors = 1;
				textureDescriptorRange[0].OffsetInDescriptorsFromTableStart = 0;
				textureDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				textureDescriptorRange[0].RegisterSpace = 0;

				D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
				desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				desc.Texture2D.PlaneSlice = 0; // @todo
				desc.Texture2D.MipSlice = 0;

				D3D12_ROOT_PARAMETER param{};
				param.ShaderVisibility = GetShaderStageDx12(slot.stage);
				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = textureDescriptorRange.GetPointer();

				descriptorRanges.Insert(textureDescriptorRange.GetPointer());

				nativeParams.Insert(param);
			}
		}
	}

	for (auto& pConst : layout->GetAllPushConstants()) {
		D3D12_ROOT_PARAMETER param{};
		param.ShaderVisibility = GetShaderStageDx12(pConst.second.stage);
		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		param.Constants.RegisterSpace = 1;
		param.Constants.ShaderRegister = pConst.second.hlslIndex;
		param.Constants.Num32BitValues = pConst.second.size / 4;

		nativeParams.Insert(param);
	}

	D3D12_ROOT_SIGNATURE_DESC description{};
	description.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	description.NumParameters = nativeParams.GetSize();
	description.pParameters = nativeParams.GetData();
	description.NumStaticSamplers = staticSamplers.GetSize();
	description.pStaticSamplers = staticSamplers.GetData();

	ComPtr<ID3DBlob> error;
	ComPtr<ID3DBlob> sign;

	GpuDx12* gpu = Engine::GetRenderer()->GetGpu()->As<GpuDx12>();

	const HRESULT result = D3D12SerializeRootSignature(&description, D3D_ROOT_SIGNATURE_VERSION_1_0, &sign, &error);
	gpu->GetDevice()->CreateRootSignature(0, sign->GetBufferPointer(), sign->GetBufferSize(), IID_PPV_ARGS(&signature));
}

ID3D12RootSignature* PipelineLayoutDx12::GetSignature() const {
	return signature.Get();
}
