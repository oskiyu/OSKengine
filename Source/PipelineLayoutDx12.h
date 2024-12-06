#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "IPipelineLayout.h"
#include "DynamicArray.hpp"

#include <d3d12.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

namespace OSK::GRAPHICS {

	class DescriptorLayoutDx12;

	class OSKAPI_CALL PipelineLayoutDx12 : public IPipelineLayout {

	public:

		PipelineLayoutDx12(const MaterialLayout* layout);

		ID3D12RootSignature* GetSignature() const;

	private:

		ComPtr<ID3D12RootSignature> signature;

	};

}

#endif
