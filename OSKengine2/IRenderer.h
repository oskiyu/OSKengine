#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "DynamicArray.hpp"

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
	class IRenderpass;
	struct PipelineCreateInfo;
	class MaterialLayout;
	class IGraphicsPipeline;
	class MaterialSystem;
	class IMaterialSlot;

	enum class RenderApiType;

	/// <summary>
	/// Proporciona la interfaz de un renderizador.
	/// Esta interfaz no implementa ning�n c�digo, se debe usar una instancia de
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
		/// <param name="appName">Nombre de la aplicaci�n / juego.</param>
		/// <param name="version">Versi�n de la aplicaci�n / juego.</param>
		/// <param name="window">Ventana enlazada.</param>
		virtual void Initialize(const std::string& appName, const Version& version, const Window& window) = 0;

		/// <summary>
		/// Cierra el renderizador.
		/// Tambi�n se llama en el destructor.
		/// </summary>
		virtual void Close() = 0;

		/// <summary>
		/// Una vez se han grabado todos los comandos, se debe iniciar su
		/// ejecuci�n en la GPU para ser renderizados.
		/// </summary>
		virtual void PresentFrame() = 0;

		ICommandList* GetCommandList() const;
		IGpuMemoryAllocator* GetMemoryAllocator() const;
		IGpu* GetGpu() const;

		/// <summary>
		/// Crea una lista de comandos para un �nico uso.
		/// �til para enviar datos a la GPU, por ejemplo.
		/// </summary>
		OwnedPtr<ICommandList> CreateSingleUseCommandList();

		/// <summary>
		/// Ejecuta el contenido de la lista de comandos.
		/// </summary>
		virtual void SubmitSingleUseCommandList(ICommandList* commandList) = 0;

		/// <summary>
		/// Castea el renderizador al tipo dado.
		/// Este tipo debe ser una implementaci�n de esta interfaz.
		/// </summary>
		template <typename T> T* As() requires std::is_base_of_v<IRenderer, T> {
			return (T*)this;
		}

		/// <summary>
		/// Crea un graphics pipeline.
		/// </summary>
		/// <param name="pipelineInfo">Configuraci�n del pipeline.</param>
		/// <param name="layout">Layout del material del pipeline.</param>
		/// <param name="renderpass">Renderpass al que estar� enlazado el pipeline.</param>
		virtual OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const IRenderpass* renderpass) = 0;

		/// <summary>
		/// Devuelve el sistema de materiales.
		/// Necesario mara manejar materiales y crear instancias de materiales.
		/// </summary>
		MaterialSystem* GetMaterialSystem() const;

		/// <summary>
		/// Crea un slot del layout del material dado.
		/// </summary>
		virtual OwnedPtr<IMaterialSlot> _CreateMaterialSlot(const std::string& name, const MaterialLayout* layout) const = 0;

		/// <summary>
		/// Devuelve el API de renderizado de bajo nivel del renderizador actual.
		/// (DX12/VULKAN/OPENGL).
		/// </summary>
		RenderApiType GetRenderApi() const;

		/// <summary>
		/// Devuelve el n�mero de im�genes del swapchain.
		/// Para recursos que necesiten tener una copia por imagen del swapchain.
		/// </summary>
		TSize GetSwapchainImagesCount() const;

	protected:

		IRenderer(RenderApiType renderApiType);

		virtual void CreateCommandQueues() = 0;
		virtual void CreateSwapchain() = 0;
		virtual void CreateSyncDevice() = 0;
		virtual void CreateGpuMemoryAllocator() = 0;
		virtual void CreateMainRenderpass() = 0;

		UniquePtr<IGpu> currentGpu;

		UniquePtr<ICommandQueue> graphicsQueue;
		UniquePtr<ICommandQueue> presentQueue;

		UniquePtr<ISwapchain> swapchain;
		UniquePtr<ISyncDevice> syncDevice;

		UniquePtr<ICommandPool> commandPool;
		UniquePtr<ICommandList> commandList;

		UniquePtr<IGpuMemoryAllocator> gpuMemoryAllocator;

		UniquePtr<IRenderpass> renderpass;
		
		UniquePtr<MaterialSystem> materialSystem;

		DynamicArray<OwnedPtr<ICommandList>> singleTimeCommandLists;

		bool isFirstRender = true;

		const Window* window = nullptr;

	private:

		RenderApiType renderApiType;

	};

}
