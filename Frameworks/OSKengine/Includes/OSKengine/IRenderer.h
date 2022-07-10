#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "DynamicArray.hpp"


#include "IGpu.h"
#include "ICommandQueue.h"
#include "ISwapchain.h"
#include "ISyncDevice.h"
#include "ICommandList.h"
#include "ICommandPool.h"
#include "IGpuMemoryAllocator.h"
#include "IRenderpass.h"
#include "MaterialSystem.h"
#include "PresentMode.h"
#include "IGpuImage.h"

// Camera 2D para renderizar a la pantalla
#include "CameraComponent2D.h"
#include "Transform2D.h"

#include <string>

namespace OSK {
	struct Version;
	namespace IO {
		class Window;
	}
}

namespace OSK::GRAPHICS {

	class MaterialLayout;
	class IMaterialSlot;
	struct PipelineCreateInfo;
	class IGraphicsPipeline;
	class IRaytracingPipeline;
	class GpuImage;
	class VertexInfo;

	enum class RenderApiType;
	enum class GpuImageLayout;

	/// <summary>
	/// Proporciona la interfaz de un renderizador.
	/// 
	/// @note Esta interfaz no implementa ning�n c�digo, se debe usar una instancia de
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
		virtual void Initialize(const std::string& appName, const Version& version, const IO::Window& window, PresentMode mode) = 0;

		/// <summary>
		/// Cierra el renderizador.
		/// Tambi�n se llama en el destructor.
		/// </summary>
		virtual void Close() = 0;

		/// <summary>
		/// Reconfigura el swapchain al haberse cambiado de tama�o
		/// la ventana.
		/// </summary>
		virtual void HandleResize();

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
		virtual OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const IRenderpass* renderpass, const VertexInfo& vertexTypeName) = 0;
		/// <summary>
		/// Crea un graphics pipeline.
		/// </summary>
		/// <param name="pipelineInfo">Configuraci�n del pipeline.</param>
		/// <param name="layout">Layout del material del pipeline.</param>
		/// <param name="renderpass">Renderpass al que estar� enlazado el pipeline.</param>
		virtual OwnedPtr<IRaytracingPipeline> _CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const IRenderpass* renderpass, const VertexInfo& vertexTypeName) = 0;

		/// <summary>
		/// Devuelve el sistema de materiales.
		/// Necesario mara manejar materiales y crear instancias de materiales.
		/// </summary>
		MaterialSystem* GetMaterialSystem() const;

		/// <summary>
		/// Crea un renderpass secundario.
		/// Renderiza para una o tres im�genes.
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
		/// Devuelve el n�mero de im�genes del swapchain.
		/// Para recursos que necesiten tener una copia por imagen del swapchain.
		/// </summary>
		TSize GetSwapchainImagesCount() const;

		/// <summary>
		/// Devuelve true si el renderizador est� inicializado y funcionando.
		/// </summary>
		bool IsOpen() const;
		
		/// <summary>
		/// True si el renderizador soporta trazado de rayos.
		/// Depende de la GPU usada.
		/// </summary>
		virtual bool SupportsRaytracing() const = 0;

		IRenderpass* GetFinalRenderpass() const;

		virtual TSize GetCurrentFrameIndex() const = 0;
		virtual TSize GetCurrentCommandListIndex() const = 0;

		/// <summary>
		/// Devuelve una c�mara 2D que renderiza en la resoluci�n
		/// completa de la pantalla.
		/// 
		/// Para poder renderizar con facilidad los diferentes render targets
		/// en los sistemas de renderizado y en IGame::BuildFrame.
		/// </summary>
		const ECS::CameraComponent2D& GetRenderTargetsCamera() const;

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
		/// @warning Si se sobreescribe, devolver� un puntero distinto al original, el cual deber� ser
		/// eliminado tras llamar a esta funci�n.</returns>
		virtual const TByte* FormatImageDataForGpu(const GpuImage* image, const TByte* data, TSize numLayers);

		virtual void CreateCommandQueues() = 0;
		virtual void CreateSwapchain(PresentMode mode) = 0;
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

		// Renderpasses
		UniquePtr<IRenderpass> finalRenderpass;
		
		UniquePtr<MaterialSystem> materialSystem;

		DynamicArray<OwnedPtr<ICommandList>> singleTimeCommandLists;

		bool isFirstRender = true;

		const IO::Window* window = nullptr;

		bool isOpen = false;

		UniquePtr<ECS::CameraComponent2D> renderTargetsCamera;
		ECS::Transform2D renderTargetsCameraTransform{ ECS::EMPTY_GAME_OBJECT };

	private:

		RenderApiType renderApiType;


	};

}
