#pragma once

#include "ISwapchain.h"

#include <wrl.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;

namespace OSK {

	enum class Format;
	class CommandQueueDx12;
	class Window;

	/// <summary>
	/// Un swapchain es una estructura encargada de manejar el cambio de imagenes que
	/// son representadas en el monitor.
	/// 
	/// La GPU trabaja en una sola imagen a la vez. El swapchain se encarga entonces
	/// de transmitir la imagen al monitor.
	/// </summary>
	class OSKAPI_CALL SwapchainDx12 : public ISwapchain {

	public:

		/// <summary>
		/// Crea el swapchain.
		/// Obtiene automáticamente el tamaño de las imágenes a partir del
		/// tamaño de la ventana.
		/// </summary>
		/// <param name="commandQueue">Al cambiar de imagen, el swapchain
		/// de DirectX 12 fuerza un flush de la cola de comandos.</param>
		void Create(Format format, const CommandQueueDx12& commandQueue, IDXGIFactory4* factory, const Window& window);

	private:

		ComPtr<IDXGISwapChain3> swapchain;

	};

}
