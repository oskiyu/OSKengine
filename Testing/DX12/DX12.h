#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "../UWP DX12/Common/d3dx12.h"

using Microsoft::WRL::ComPtr;

bool IsDxSuccess(HRESULT result) {
	return result >= 0;
}

#define SET_DX12_OBJECT_NAME(object) object->SetName(L#object);
