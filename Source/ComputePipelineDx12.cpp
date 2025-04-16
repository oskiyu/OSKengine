#include "ComputePipelineDx12.h"

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "PipelineLayoutDx12.h"
#include "PipelinesExceptions.h"
#include "OSKengine.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ComputePipelineDx12::ComputePipelineDx12(const PipelineCreateInfo& info, const MaterialLayout* materialLayout, IGpu* device) {
	layout = MakeUnique<PipelineLayoutDx12>(materialLayout);

	computeShader = LoadShader(info, {}, info.computeShaderPath, ShaderStage::COMPUTE, *materialLayout);

	D3D12_COMPUTE_PIPELINE_STATE_DESC createInfo{};
	createInfo.CS = computeShader.bytecode;
	createInfo.pRootSignature = layout->As<PipelineLayoutDx12>()->GetSignature();
	createInfo.NodeMask = 0;
	createInfo.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	const HRESULT result = device->As<GpuDx12>()->GetDevice()->CreateComputePipelineState(&createInfo, IID_PPV_ARGS(&dxPipeline));
	OSK_ASSERT(SUCCEEDED(result), PipelineCreationException(result));
}

#endif
