#include "PipelineLayoutDx12.h"

#include "MaterialLayout.h"
#include "ShaderBindingType.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuDx12.h"

using namespace OSK;

D3D12_SHADER_VISIBILITY GetShaderStageDx12(ShaderStage stage) {
	if (OSK::EFTraits::HasFlag(stage, ShaderStage::VERTEX)
		&& OSK::EFTraits::HasFlag(stage, ShaderStage::FRAGMENT))
		return D3D12_SHADER_VISIBILITY_ALL;

	switch (stage) {
	case OSK::ShaderStage::VERTEX:
		return D3D12_SHADER_VISIBILITY_VERTEX;
	case OSK::ShaderStage::FRAGMENT:
		return D3D12_SHADER_VISIBILITY_PIXEL;
	default:
		return D3D12_SHADER_VISIBILITY_ALL;
	}
}

D3D12_ROOT_PARAMETER_TYPE GetParamTypeDx12(ShaderBindingType type) {
	switch (type) {
	case OSK::ShaderBindingType::UNIFORM_BUFFER:
		return D3D12_ROOT_PARAMETER_TYPE_CBV;
	case OSK::ShaderBindingType::TEXTURE:
		//break;
	default:
		return D3D12_ROOT_PARAMETER_TYPE_CBV;
	}
}


PipelineLayoutDx12::PipelineLayoutDx12(const MaterialLayout* layout)
	: IPipelineLayout(layout) {
	
	DynamicArray<D3D12_ROOT_PARAMETER> nativeParams;
	for (auto& setName : layout->GetAllSlotNames()) {
		auto& set = layout->GetSlot(setName);

		for (auto& binding : set.bindings) {
			D3D12_ROOT_PARAMETER param{};
			param.ShaderVisibility = GetShaderStageDx12(set.stage);
			param.ParameterType = GetParamTypeDx12(binding.second.type);
			param.Descriptor.RegisterSpace = 0;
			param.Descriptor.ShaderRegister = binding.second.hlslIndex;
			
			nativeParams.Insert(param);
		}
	}

	D3D12_ROOT_SIGNATURE_DESC description{};
	description.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	description.NumParameters = nativeParams.GetSize();
	description.pParameters = nativeParams.GetData();
	description.NumStaticSamplers = 0;
	description.pStaticSamplers = NULL;

	ComPtr<ID3DBlob> error;
	ComPtr<ID3DBlob> sign;

	GpuDx12* gpu = Engine::GetRenderer()->GetGpu()->As<GpuDx12>();

	HRESULT result = D3D12SerializeRootSignature(&description, D3D_ROOT_SIGNATURE_VERSION_1_0, &sign, &error);
	gpu->GetDevice()->CreateRootSignature(0, sign->GetBufferPointer(), sign->GetBufferSize(), IID_PPV_ARGS(&signature));
}

ID3D12RootSignature* PipelineLayoutDx12::GetSignature() const {
	return signature.Get();
}
