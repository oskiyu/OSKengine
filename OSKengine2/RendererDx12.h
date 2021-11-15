#pragma once

#include <string>
#include "OSKmacros.h"
#include "IRenderer.h"
#include "UniquePtr.hpp"

#include <wrl.h>
using namespace Microsoft::WRL;
#include <d3d12.h>
#include <dxgi1_6.h>

namespace OSK {

	class CommandQueueDx12;

	/// <summary>
	/// Implementación de la interfaz para el renderizador de DirectX 12.
	/// </summary>
	class RendererDx12 : public IRenderer {

	public:

		~RendererDx12();

		void Initialize(const std::string& appName, const Version& version, const Window& window) override;
		void Close() override;

	protected:

		void CreateCommandQueues() override;
		void CreateSwapchain() override;

	private:

		void ChooseGpu();

		/// <summary>
		/// Se usa para crear cosas de dx12.
		/// </summary>
		ComPtr<IDXGIFactory4> factory;
		ComPtr<ID3D12Debug> debugConsole;

	};

}
