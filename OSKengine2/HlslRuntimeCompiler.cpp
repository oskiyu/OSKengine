#include "HlslRuntimeCompiler.h"

#include "Assert.h"
#include "DynamicArray.hpp"
#include "StringOperations.h"

#include "PipelinesExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ComPtr<IDxcCompiler3> HlslRuntimeCompiler::compiler = nullptr;
ComPtr<IDxcUtils> HlslRuntimeCompiler::utils = nullptr;
ComPtr<IDxcIncludeHandler> HlslRuntimeCompiler::includeHandler = nullptr;

void HlslRuntimeCompiler::InitializeComponents() {
	HRESULT result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	OSK_ASSERT(SUCCEEDED(result), ShaderCompilingException("No se pudo instanciar la libreria del compilador DX12."));

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()));
	OSK_ASSERT(SUCCEEDED(result), ShaderCompilingException("No se pudo instanciar el compilador DX12."));

	result = utils->CreateDefaultIncludeHandler(&includeHandler);
	OSK_ASSERT(SUCCEEDED(result), ShaderCompilingException("No se pudo instanciar el iclude handler compilador DX12."));
}

ComPtr<IDxcBlob> HlslRuntimeCompiler::CompileFromFile(const std::string& path, const std::string& hlslProfile) {
	// Prerpocesar el código
	ComPtr<IDxcBlobEncoding> sourceBlob;
	static UINT32 encoding = CP_UTF8;

	HRESULT result = utils->LoadFile(L"./temp_shader", &encoding, sourceBlob.GetAddressOf());
	OSK_ASSERT(SUCCEEDED(result), ShaderCompilingException(path));

	// Región del shader compilado
	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
	sourceBuffer.Size = sourceBlob->GetBufferSize();
	sourceBuffer.Encoding = DXC_CP_ACP;

	// Argumentos
	std::wstring profile = StringToWideString(hlslProfile);
	DynamicArray<LPCWSTR> args;
	args.Insert(L"-E");
	args.Insert(L"main");
	args.Insert(L"-T");
	args.Insert(profile.c_str());

	// Compilación
	ComPtr<IDxcResult> compiled;
	result = compiler->Compile(&sourceBuffer, args.GetData(), args.GetSize(), includeHandler.Get(), IID_PPV_ARGS(&compiled));

	ComPtr<IDxcBlobUtf8> compilationErrors;
	compiled->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(compilationErrors.GetAddressOf()), nullptr);
	if (compilationErrors && compilationErrors->GetStringLength() > 0)
		OSK_ASSERT(false, ShaderCompilingException(std::string((char*)compilationErrors->GetBufferPointer())));

	ComPtr<IDxcBlob> output;

	compiled->GetResult(&output);

	return output;
}
