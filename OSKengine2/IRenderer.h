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
#include "RenderTarget.h"
#include "MaterialSystem.h"
#include "PresentMode.h"

#include "IGpuImage.h"
#include "IGpuDataBuffer.h"

// Camera 2D para renderizar a la pantalla
#include "CameraComponent2D.h"
#include "Transform2D.h"

#include <string>
#include <functional>

namespace OSK {
	struct Version;
	namespace IO {
		class IDisplay;
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
	class IPostProcessPass;

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

		OSK_DEFINE_AS(IRenderer);

		virtual ~IRenderer() = default;

		/// @brief Inicializa todo el sistema de renderizado.
		/// @param appName Nombre de la aplicación / juego.
		/// @param version Versión de la aplicación / juego.
		/// @param display Ventana enlazada (sobre la que se renderizará el juego).
		/// @param mode Modo de sincronización vertical.
		virtual void Initialize(
			const std::string& appName, 
			const Version& version, 
			const IO::IDisplay& display, 
			PresentMode mode) = 0;

		/// <summary> Cierra el renderizador. </summary>
		/// 
		/// @note También se llama en el destructor.
		virtual void Close() = 0;

		/// <summary> Reconfigura el swapchain al cambiar de tamaño la ventana. </summary>
		virtual void HandleResize();

		/// <summary>
		/// Una vez se han grabado todos los comandos, se debe iniciar su
		/// ejecución en la GPU para ser renderizados.
		/// </summary>
		virtual void PresentFrame() = 0;

		/// @brief Hace que el hilo actual espere a que se hayan ejecutado
		/// todos los comandos en la GPU.
		/// Necesario si se van a eliminar recursos que puede que todavía
		/// estén en uso.
		virtual void WaitForCompletion() = 0;

#pragma region Getters

		ICommandList* GetPreComputeCommandList() const;
		ICommandList* GetGraphicsCommandList() const;
		ICommandList* GetPostComputeCommandList() const;
		ICommandList* GetFrameBuildCommandList() const;

		inline ICommandQueue* GetGraphicsCommandQueue() const { return graphicsQueue.GetPointer(); }
		inline ICommandQueue* GetComputeCommandQueue() const { return computeQueue.GetPointer(); }
		inline ICommandQueue* GetPresentCommandQueue() const { return presentQueue.GetPointer(); }

		/// <summary> Devuelve un puntero al asignador de memoria de la GPU. </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @note Es un puntero estable.
		/// @warning Será nulo si no se ha llamado a IRenderer::Initialize.
		IGpuMemoryAllocator* GetAllocator() const;

		/// <summary> Devuelve un puntero al objeto Gpu. </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @note Es un puntero estable.
		/// @warning Será nulo si no se ha llamado a IRenderer::Initialize.
		IGpu* GetGpu() const;

		/// <summary> Devuelve un puntero al sistema de materiales. </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @note Es un puntero estable.
		/// @warning Será nulo si no se ha llamado a IRenderer::Initialize.
		MaterialSystem* GetMaterialSystem() const;

		/// <summary> Devuelve el swapchain de la aplicación. </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @note Es un puntero estable.
		/// @warning Será nulo si no se ha llamado a IRenderer::Initialize.
		/// @warning Función interna, no usar.
		ISwapchain* _GetSwapchain() const;


		/// <summary> Devuelve el render target que renderiza sobre la imagen final del swapchain. </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @note Es un puntero estable.
		/// @warning Será nulo si no se ha llamado a IRenderer::Initialize.
		RenderTarget* GetFinalRenderTarget() const;

		/// <summary>
		/// Devuelve una cámara 2D que renderiza en la resolución
		/// completa de la pantalla.
		/// 
		/// Para poder renderizar con facilidad los diferentes render targets
		/// en los sistemas de renderizado y en IGame::BuildFrame.
		/// </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		const ECS::CameraComponent2D& GetRenderTargetsCamera() const;


		/// <summary> Devuelve true si el renderizador está inicializado y funcionando. </summary>
		bool IsOpen() const;

		/// <summary>
		/// True si el renderizador soporta trazado de rayos.
		/// Depende de la GPU usada.
		/// </summary>
		virtual bool SupportsRaytracing() const = 0;


		/// <summary>
		/// Devuelve el número de imágenes del swapchain.
		/// Para recursos que necesiten tener una copia por imagen del swapchain.
		/// </summary>
		TSize GetSwapchainImagesCount() const;

		/// <summary> Devuelve el índidce del próximo fotograma que será presentado. </summary>
		virtual TSize GetCurrentFrameIndex() const = 0;

		/// <summary> Devuelve el índidce de la lista de comandos que será usada en el prximo fotograma. </summary>
		virtual TSize GetCurrentCommandListIndex() const = 0;

		/// <summary> Devuelve el índidce de los recursos procesados en el fotograma actual. </summary>
		TIndex GetCurrentResourceIndex() const;


		/// <summary>
		/// Devuelve el API de renderizado de bajo nivel del renderizador actual.
		/// (DX12/VULKAN/OPENGL).
		/// </summary>
		RenderApiType GetRenderApi() const;

#pragma endregion


#pragma region Image upload

		/// <summary> Rellena la imagen en la GPU con los datos dados. </summary>
		/// 
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		void UploadLayeredImageToGpu(
			GpuImage* destination, 
			const TByte* data, 
			TSize numBytes, 
			TSize numLayers, 
			ICommandList* cmdList);

		/// <summary> Rellena la imagen en la GPU con los datos dados. </summary>
		/// 
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		void UploadImageToGpu(
			GpuImage* destination, 
			const TByte* data, 
			TSize numBytes, 
			ICommandList* cmdList);

		/// <summary>
		/// Rellena la imagen en la GPU con los datos dados.
		/// </summary>
		/// 
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		void UploadCubemapImageToGpu(
			GpuImage* destination, 
			const TByte* data, 
			TSize numBytes, 
			ICommandList* cmdList);

#pragma endregion

#pragma region Factory methods

		virtual OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout, 
			const VertexInfo& vertexTypeName) = 0;

		virtual OwnedPtr<IRaytracingPipeline> _CreateRaytracingPipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout, 
			const VertexInfo& vertexTypeName) = 0;

		virtual OwnedPtr<IComputePipeline> _CreateComputePipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout) = 0;

		virtual OwnedPtr<IMaterialSlot> _CreateMaterialSlot(
			const std::string& name, 
			const MaterialLayout& layout) const = 0;

		/// <summary> Crea una lista de comandos para un único uso. </summary>
		OwnedPtr<ICommandList> CreateSingleUseCommandList();
		/// <summary> Ejecuta el contenido de la lista de comandos. </summary>
		virtual void SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) = 0;

#pragma endregion


		/// <summary>
		/// Registra un render target que debe cambiar de tamaño cuando la ventana
		/// cambie de tamaño.
		/// </summary>
		/// 
		/// @note El render target debe tener estabilidad de puntero.
		void RegisterRenderTarget(RenderTarget* renderTarget);

		/// <summary>
		/// Quita un render target de la lista de render targers que deben cambiar
		/// de tamaño cuando la ventana cambie de tamaño.
		/// </summary>
		/// 
		/// @note Si el render target no estaba registrado, no ocurrirá nada.
		void UnregisterRenderTarget(RenderTarget* renderTarget);


		/// <summary>
		/// Registra un efecto de postprocesamiento que debe cambiar de tamaño cuando la ventana
		/// cambie de tamaño.
		/// </summary>
		/// 
		/// @note El render target debe tener estabilidad de puntero.
		void RegisterPostProcessingPass(IPostProcessPass* renderTarget);

		/// <summary>
		/// Quita un efecto de postprocesamiento de la lista de efectos de postprocesamiento que deben cambiar
		/// de tamaño cuando la ventana cambie de tamaño.
		/// </summary>
		/// 
		/// @note Si el efecto de postprocesamiento no estaba registrado, no ocurrirá nada.
		void UnregisterPostProcessingPass(IPostProcessPass* renderTarget);


		/// <summary>
		/// True si la funcionalidad de raytracing está activa.
		/// </summary>
		bool IsRtActive() const;

		/// <summary>
		/// Añade una función que se ejecutará cuando cambie de tamaño la ventana.
		/// </summary>
		void AddResizeCallback(std::function<void(const Vector2ui&)> callback);

		/// <summary>
		/// True si el cambio de tamaño de la ventana se maneja implícitamente
		/// en el renderizador.
		/// </summary>
		bool _HasImplicitResizeHandling() const;


		Material* GetFullscreenRenderingMaterial() const;

	protected:

		IRenderer(RenderApiType renderApiType, bool requestRayTracing);

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
		virtual void CreateSwapchain(PresentMode mode) = 0;
		virtual void CreateSyncDevice() = 0;
		virtual void CreateGpuMemoryAllocator() = 0;
		
		void CreateMainRenderpass();

		UniquePtr<IGpu> currentGpu;

		UniquePtr<ICommandQueue> graphicsQueue;
		UniquePtr<ICommandQueue> computeQueue;
		UniquePtr<ICommandQueue> presentQueue;

		UniquePtr<ISwapchain> swapchain;
		UniquePtr<ISyncDevice> syncDevice;

		UniquePtr<ICommandPool> graphicsCommandPool;
		UniquePtr<ICommandList> graphicsCommandList;
		UniquePtr<ICommandList> frameBuildCommandList;

		UniquePtr<ICommandPool> computeCommandPool;
		UniquePtr<ICommandList> preComputeCommandList;
		UniquePtr<ICommandList> postComputeCommandList;

		UniquePtr<IGpuMemoryAllocator> gpuMemoryAllocator;

		// Renderpasses
		UniquePtr<RenderTarget> finalRenderTarget;
		
		UniquePtr<MaterialSystem> materialSystem;

		DynamicArray<UniquePtr<ICommandList>> singleTimeCommandLists;

		LinkedList<std::function<void(const Vector2ui&)>> resizeCallbacks;

		bool isFirstRender = true;

		const IO::IDisplay* display = nullptr;

		bool isOpen = false;

		UniquePtr<ECS::CameraComponent2D> renderTargetsCamera;
		ECS::Transform2D renderTargetsCameraTransform{ ECS::EMPTY_GAME_OBJECT };

		bool implicitResizeHandling = false;

		/// <summary>
		/// Contiene render targets que deben cambiar de tamaño cuando la ventana
		/// cambie de tamaño.
		/// </summary>
		DynamicArray<RenderTarget*> resizableRenderTargets;
		DynamicArray<IPostProcessPass*> resizablePostProcessingPasses;

		bool isRtActive = false;

		bool IsRtRequested() const;

		// True si se usa la misma queue para enviar comandos gráficos y de computación.
		bool singleCommandQueue = false;

	private:

		RenderApiType renderApiType;
		bool isRtRequested = false;

	};

}
