#include "GpuDx12.h"

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "CommandPoolDx12.h"

#include "Assert.h"
#include "NotImplementedException.h"

#include "CommandsSupport.h"
#include "GpuQueueTypes.h"

#include "OSKengine.h"
#include "Logger.h"

#include <atlconv.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuDx12::~GpuDx12() {
	Close();
}

void GpuDx12::Close() {

}

GpuMemoryUsageInfo GpuDx12::GetMemoryUsageInfo() const {
	OSK_ASSERT(false, NotImplementedException());
	return {};
}

OwnedPtr<ICommandPool> GpuDx12::CreateGraphicsCommandPool() {
	return new CommandPoolDx12(
		this,
		CommandsSupport::COMPUTE |
		CommandsSupport::GRAPHICS |
		CommandsSupport::PRESENTATION |
		CommandsSupport::TRANSFER,
		GpuQueueType::MAIN);
}

OwnedPtr<ICommandPool> GpuDx12::CreateComputeCommandPool() {
	return new CommandPoolDx12(
		this,
		CommandsSupport::COMPUTE |
		CommandsSupport::GRAPHICS |
		CommandsSupport::PRESENTATION |
		CommandsSupport::TRANSFER,
		GpuQueueType::MAIN);
}

std::optional<OwnedPtr<ICommandPool>> GpuDx12::CreateTransferOnlyCommandPool() {
	return new CommandPoolDx12(this, CommandsSupport::TRANSFER, GpuQueueType::ASYNC_TRANSFER);
}

OwnedPtr<IGpuImageSampler> GpuDx12::CreateSampler(const GpuImageSamplerDesc& info) const {
	OSK_ASSERT(false, NotImplementedException());
	return nullptr;
}

ComPtr<IDXGIAdapter4> GpuDx12::ChooseDeviceAdapter(ComPtr<IDXGIFactory4> factory) {
	// Adaptador temporal, usado para enumerar los adaptadores disponibles.
	ComPtr<IDXGIAdapter1> adapter1;
	// Adaptador final.
	ComPtr<IDXGIAdapter4> output;

	bool found = false;
	for (USize32 i = 0; !found && factory->EnumAdapters1(i, &adapter1) != DXGI_ERROR_NOT_FOUND; i++) {

		// Descripción de la GPU.
		DXGI_ADAPTER_DESC1 description{};
		adapter1->GetDesc1(&description);

		// Que este adaptador no sea emulado por software.
		if (description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}

		// Comprobar si es compatible con las características deseadas.
		auto isValid = D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(isValid)) {
			// Engine::GetLogger()->InfoLog("GPU elegida: " + std::string(Microsoft::WRL::CW2A(description.Description)));
			found = true;
		}
	}

	// Obtener el adapter4.
	adapter1.As(&output);

	return output;
}

void GpuDx12::CreateDevice(ComPtr<IDXGIAdapter4> adapter, bool useDebug) {
	m_adapter = adapter;

	D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device));

	if (useDebug) {
		ID3D12InfoQueue* infoQueue = nullptr;
		HRESULT result = m_device->QueryInterface(IID_PPV_ARGS(&infoQueue));
		if (SUCCEEDED(result)) {
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			// DWORD cookie = 0;
			// infoQueue1->RegisterMessageCallback(RendererDx12::DebugCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, this, &cookie);
		}
		else {
			Engine::GetLogger()->InfoLog("NO se ha podido establecer el callback de mensajes de log.");
		}
	}
}

IDXGIAdapter1* GpuDx12::GetAdapter() const {
	return m_adapter.Get();
}

ID3D12Device* GpuDx12::GetDevice() const {
	return m_device.Get();
}

#endif
