#include "IPipelineDx12.h"

#include <d3dcompiler.h>

#undef min

#include "FileIO.h"
#include "SpirvToHlsl.h"
#include "WindowsUtils.h"

using namespace OSK;
using namespace OSK::IO;
using namespace OSK::GRAPHICS;

ID3D12PipelineState* IPipelineDx12::GetPipeline() const {
    return dxPipeline.Get();
}

ComPtr<ID3DBlob> IPipelineDx12::LoadBlob(LPCWSTR filename) {
	ComPtr<ID3DBlob> shaderBlob;
	const HRESULT hr = D3DReadFileToBlob(filename, &shaderBlob);

	if (FAILED(hr))
		OSK_ASSERT(false, "No se pudo compilar el shader. Code: " + std::to_string(hr));

	return shaderBlob;
}

ComPtr<IDxcBlob> IPipelineDx12::CompileBlob(const std::string& spirvPath, const VertexInfo& vertexInfo, const std::string& hlslProfile, const MaterialLayout& layout) {
	const auto bytecode = FileIO::ReadBinaryFromFile(spirvPath);

	// Generación del código HLSL.
	SpirvToHlsl compilerToHlsl = SpirvToHlsl(bytecode);
	compilerToHlsl.SetHlslTargetProfile(6, 1);
	compilerToHlsl.SetVertexAttributesMapping(vertexInfo);
	compilerToHlsl.SetLayoutMapping(layout);

	const std::string hlslSourceCode = compilerToHlsl.CreateHlsl();

	//Engine::GetLogger()->DebugLog(hlslSourceCode);
	FileIO::WriteFile("./temp_shader", hlslSourceCode);

	HlslRuntimeCompiler compiler;
	return compiler.CompileFromFile("./temp_shader", hlslProfile);
}

ShaderStageDx12 IPipelineDx12::LoadShader(const PipelineCreateInfo& info, const VertexInfo& vertexInfo, const std::string& path, ShaderStage stage, const MaterialLayout& mLayout) {
	ShaderStageDx12 output{};

	if (info.precompiledHlslShaders) {
		const auto wPath = StringToWideString(path);
		output.isBlobPrecompiled = true;
		output.precompiled = LoadBlob(wPath.c_str());

		output.bytecode.BytecodeLength = output.precompiled->GetBufferSize();
		output.bytecode.pShaderBytecode = output.precompiled->GetBufferPointer();
	}
	else {
		output.isBlobPrecompiled = false;
		output.spirvCompiled = CompileBlob(path, vertexInfo, GetHlslProfile(stage), mLayout);

		output.bytecode.BytecodeLength = output.spirvCompiled->GetBufferSize();
		output.bytecode.pShaderBytecode = output.spirvCompiled->GetBufferPointer();
	}

	return output;
}

std::string IPipelineDx12::GetHlslProfile(ShaderStage stage) {
	switch (stage) {
	case OSK::GRAPHICS::ShaderStage::VERTEX:
		return "vs_6_1";
	case OSK::GRAPHICS::ShaderStage::FRAGMENT:
		return "ps_6_1";
	case OSK::GRAPHICS::ShaderStage::TESSELATION_CONTROL:
		return "hs_6_1";
	case OSK::GRAPHICS::ShaderStage::TESSELATION_EVALUATION:
		return "ds_6_1";
	case OSK::GRAPHICS::ShaderStage::RT_RAYGEN:
		return "lib_6_3";
	case OSK::GRAPHICS::ShaderStage::RT_CLOSEST_HIT:
		return "lib_6_3";
	case OSK::GRAPHICS::ShaderStage::RT_MISS:
		return "lib_6_3";
	case OSK::GRAPHICS::ShaderStage::COMPUTE:
		return "cs_6_1";

	default:
		OSK_ASSERT(false, "Shader stage " + ToString(stage) + " no soportado en DX12.");
		break;
	}
}
