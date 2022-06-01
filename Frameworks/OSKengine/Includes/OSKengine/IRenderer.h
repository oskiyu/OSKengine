#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "DynamicArray.hpp"

#include <string>

namespace OSK {
	struct Version;
	namespace IO {
		class Window;
	}
}

namespace OSK::GRAPHICS {

	class MaterialSystem;
	class MaterialLayout;
	class IMaterialSlot;
	class IGpu;
	class ICommandPool;
	class ICommandList;
	class ISwapchain;
	class ICommandQueue;
	class ISyncDevice;
	class IGpuMemoryAllocator;
	class IRenderpass;
	struct PipelineCreateInfo;
	class IGraphicsPipeline;
	class GpuImage;
	class VertexInfo;

	enum class RenderApiType;
	enum class GpuImageLayout;

	/// <summary>
	/// Proporciona la interfaz de un renderizador.
	/// 
	/// @note Esta interfaz no implementa ningún código, se debe usar una instancia de
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
		virtual void Initialize(const std::string& appName, const Version& version, const IO::Window& window) = 0;

		/// <summary>
		/// Cierra el renderizador.
		/// También se llama en el destructor.
		/// </summary>
		virtual void Close() = 0;

		/// <summary>
		/// Reconfigura el swapchain al haberse cambiado de tamaño
		/// la ventana.
		/// </summary>
		virtual void HandleResize() = 0;

		/// <summary>
		/// Una vez se han grabado todos los comandos, se debe iniciar su
		/// ejecución en la GPU para ser renderizados.
		/// </summary>
		virtual void PresentFrame() = 0;

		ICommandList* GetCommandList() const;
		IGpuMemoryAllocator* GetMemoryAllocator() const;
		IGpu* GetGpu() const;

		/// <summary>
		/// Crea una lista de comandos para un único uso.
		/// Útil para enviar datos a la GPU, por ejemplo.
		/// </summary>
		OwnedPtr<ICommandList> CreateSingleUseCommandList();

		/// <summary>
		/// Ejecuta el contenido de la lista de comandos.
		/// </summary>
		virtual void SubmitSingleUseCommandList(ICommandList* commandList) = 0;

		/// <summary>
		/// Rellena la imagen en la GPU con los datos dados.
		/// </summary>
		/// 
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		void UploadLayeredImageToGpu(GpuImage* destination, const TByte* data, TSize numBytes, GpuImageLayout finalLayout, TSize numLayers);

		/// <summary>
		/// Rellena la imagen en la GPU con los datos dados.
		/// </summary>
		/// 
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		void UploadImageToGpu(GpuImage* destination, const TByte* data, TSize numBytes, GpuImageLayout finalLayout);

		/// <summary>
		/// Rellena la imagen en la GPU con los datos dados.
		/// </summary>
		/// 
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		void UploadCubemapImageToGpu(GpuImage* destination, const TByte* data, TSize numBytes, GpuImageLayout finalLayout);

		/// <summary>
		/// Castea el renderizador al tipo dado.
		/// Este tipo debe ser una implementación de esta interfaz.
		/// </summary>
		template <typename T> T* As() requires std::is_base_of_v<IRenderer, T> {
			return (T*)this;
		}

		/// <summary>
		/// Crea un graphics pipeline.
		/// </summary>
		/// <param name="pipelineInfo">Configuración del pipeline.</param>
		/// <param name="layout">Layout del material del pipeline.</param>
		/// <param name="renderpass">Renderpass al que estará enlazado el pipeline.</param>
		virtual OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const IRenderpass* renderpass, const VertexInfo& vertexTypeName) = 0;

		/// <summary>
		/// Devuelve el sistema de materiales.
		/// Necesario mara manejar materiales y crear instancias de materiales.
		/// </summary>
		MaterialSystem* GetMaterialSystem() const;

		/// <summary>
		/// Crea un renderpass secundario.
		/// Renderiza para una o tres imágenes.
		/// Para que una sola imagen sea renderizada, las otras dos deben ser NULL.
		/// </summary>
		virtual OwnedPtr<IRenderpass> CreateSecondaryRenderpass(GpuImage* targetImage0, GpuImage* targetImage1, GpuImage* targetImage2) = 0;

		/// <summary>
		/// Crea un renderpass secundario.
		/// </summary>
		/// <param name="targetImage">Imagen sobre la que se va a renderizar.</param>
		OwnedPtr<IRenderpass> CreateSecondaryRenderpass(GpuImage* targetImage);

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
		/// Devuelve el número de imágenes del swapchain.
		/// Para recursos que necesiten tener una copia por imagen del swapchain.
		/// </summary>
		TSize GetSwapchainImagesCount() const;

		/// <summary>
		/// Devuelve true si el renderizador está inicializado y funcionando.
		/// </summary>
		bool IsOpen() const;

		IRenderpass* GetMainRenderpass() const;

		virtual TSize GetCurrentFrameIndex() const = 0;
		virtual TSize GetCurrentCommandListIndex() const = 0;

	protected:

		IRenderer(RenderApiType renderApiType);

		/// <summary>
		/// Debido a razones de alineamiento, es posible que se tengan que desplazar partes de la imagen
		/// para que se pueda realizar correctamente una copia de la imagen a la GPU.
		/// 
		/// Por defecto, devuelve el puntero original.
		/// </summary>
		/// <param name="image">Imagen para la que se va a formatear los datos.</param>
		/// <param name="data">Datos originales.</param>
		/// <returns>Datos con formato.
		/// 
		/// @warning Si se sobreescribe, devolverá un puntero distinto al original, el cual deberá ser
		/// eliminado tras llamar a esta función.</returns>
		virtual const TByte* FormatImageDataForGpu(const GpuImage* image, const TByte* data, TSize numLayers);

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

		const IO::Window* window = nullptr;

		bool isOpen = false;

	private:

		RenderApiType renderApiType;

	};

}
