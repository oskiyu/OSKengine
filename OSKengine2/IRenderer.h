#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"

#include <string>

namespace OSK {

	class Window;
	struct Version;
	class IGpu;
	class ICommandPool;
	class ICommandList;
	class ISwapchain;
	class ICommandQueue;
	class ISyncDevice;
	class IGpuMemoryAllocator;

	/// <summary>
	/// Proporciona la interfaz de un renderizador.
	/// Esta interfaz no implementa ningún código, se debe usar una instancia de
	/// RendererVulkan o RendererDx12.
	/// </summary>
	class OSKAPI_CALL IRenderer {

	public:

		/// <summary>
		/// Destruye el renderizador.
		/// </summary>
		virtual ~IRenderer() = default;

		/// <summary>
		/// Inicializa todo el sistema de renderizado.
		/// </summary>
		/// <param name="appName">Nombre de la aplicación / juego.</param>
		/// <param name="version">Versión de la aplicación / juego.</param>
		/// <param name="window">Ventana enlazada.</param>
		virtual void Initialize(const std::string& appName, const Version& version, const Window& window) = 0;

		/// <summary>
		/// Cierra el renderizador.
		/// También se llama en el destructor.
		/// </summary>
		virtual void Close() = 0;

		/// <summary>
		/// Una vez se han grabado todos los comandos, se debe iniciar su
		/// ejecución en la GPU para ser renderizados.
		/// </summary>
		virtual void PresentFrame() = 0;

		ICommandList* GetCommandList() const;
		IGpuMemoryAllocator* GetMemoryAllocator() const;
		IGpu* GetGpu() const;

		/// <summary>
		/// Castea el renderizador al tipo dado.
		/// Este tipo debe ser una implementación de esta interfaz.
		/// </summary>
		template <typename T> T* As() requires std::is_base_of_v<IRenderer, T> {
			return (T*)this;
		}

	protected:

		IRenderer() { }

		virtual void CreateCommandQueues() = 0;
		virtual void CreateSwapchain() = 0;
		virtual void CreateSyncDevice() = 0;
		virtual void CreateGpuMemoryAllocator() = 0;

		UniquePtr<IGpu> currentGpu;

		UniquePtr<ICommandQueue> graphicsQueue;
		UniquePtr<ICommandQueue> presentQueue;

		UniquePtr<ISwapchain> swapchain;
		UniquePtr<ISyncDevice> syncDevice;

		UniquePtr<ICommandPool> commandPool;
		UniquePtr<ICommandList> commandList;

		UniquePtr<IGpuMemoryAllocator> gpuMemoryAllocator;

		bool isFirstRender = true;

		const Window* window = nullptr;

	private:

	};

}
