#pragma once

#include "ApiCall.h"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "DynamicArray.hpp"

// Comandos y colas
#include "ICommandQueue.h"
#include "ICommandList.h"
#include "ICommandPool.h"
#include "ThreadedCommandPoolMap.h"

// Otras interfaces
#include "IGpu.h"
#include "ISwapchain.h"
#include "IGpuMemoryAllocator.h"

// Materiales
#include "MaterialSystem.h"
#include "Material.h"

// Sincronización.
#include "MutexHolder.h"

// Recursos
#include "IGpuImage.h"
#include "GpuBuffer.h"
#include "RenderTarget.h"

#include "IGpuImageSampler.h"

// Registro de render-passes
#include "ShaderPassFactory.h"

// Otros
#include "PresentMode.h"

#include <string>


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


	/// @brief Clase singleton de la arquitectura de renderizado.
	class OSKAPI_CALL IRenderer {

	public:

		OSK_DEFINE_AS(IRenderer);

		virtual ~IRenderer() = default;

		/// @brief Inicializa todo el sistema de renderizado.
		/// @param appName Nombre de la aplicación / juego.
		/// @param version Versión de la aplicación / juego.
		/// @param display Ventana enlazada (sobre la que se renderizará el juego).
		/// @param mode Modo de sincronización vertical.
		/// 
		/// @throws GpuNotFoundException Si no se encuentra ninguna GPU compatible.
		/// @throws LogicalDeviceCreationException Si ocurre algún error al establecer la conexión lógica con la GPU elegida.
		/// @throws CommandPoolCreationException Si ocurre algún error al crear las pools de comandos.
		/// @throws RendererCreationException Si ocurre algún otro error.
		virtual void Initialize(
			const std::string& appName, 
			const Version& version, 
			const IO::IDisplay& display, 
			PresentMode mode) = 0;

		/// @brief Cierra el renderizador.
		/// @note También se llama en el destructor.
		virtual void Close() = 0;

		/// @brief Cierra los siguientes elementos del renderizador:
		/// - Swapchain.
		/// - Render target final.
		/// - Sistema de materiales.
		/// - Alojador de memoria VRAM.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		void CloseSingletonInstances();

		/// @brief Cierra la GPU.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		void CloseGpu();


		/// @brief Cambia el modo de presentación del renderizador.
		/// @param mode Modo de sincronización vertical.
		void SetPresentMode(PresentMode mode);

		/// @brief Reconfigura el swapchain al cambiar de tamaño la ventana.
		/// @param resolution Nuevo tamaño de la imagen.
		virtual void HandleResize(const Vector2ui& resolution);

		/// @brief Una vez se han grabado todos los comandos, se debe iniciar su
		/// ejecución en la GPU para ser renderizados.
		/// 
		/// @throws CommandListSubmitException Si ocurre algún error al mandar las listas.
		/// @throws CommandQueueSubmitException Si ocurre algún error al ejecutar las colas.
		virtual void PresentFrame() = 0;

		/// @brief Hace que el hilo actual espere a que se hayan ejecutado
		/// todos los comandos en la GPU.
		/// Necesario si se van a eliminar recursos que puede que todavía
		/// estén en uso.
		virtual void WaitForCompletion() = 0;

#pragma region Getters

		ShaderPassFactory* GetShaderPassFactory();

		/// @return Puntero al asignador de memoria de la GPU.
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @stablepointer.
		IGpuMemoryAllocator* GetAllocator();

		/// @return Puntero al objeto Gpu.
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @stablepointer
		IGpu* GetGpu();

		/// @return Puntero al objeto Gpu.
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @stablepointer
		const IGpu* GetGpu() const;

		/// @return Puntero al sistema de materiales.
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @stablepointer
		MaterialSystem* GetMaterialSystem();

		/// @return Swapchain de la aplicación.
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @stablepointer
		/// @warning Función interna, no usar.
		ISwapchain* _GetSwapchain();


		/// @return Devuelve el render target que renderiza sobre la imagen final del swapchain.
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @stablepointer
		RenderTarget* GetFinalRenderTarget();


		/// @return True si el renderizador soporta trazado de rayos.
		/// Depende de la GPU usada.
		/// @pre Se ha llamado a IRenderer::Initialize.
		virtual bool SupportsRaytracing() const = 0;


		/// @return Devuelve el número de imágenes del swapchain.
		/// Para recursos que necesiten tener una copia por imagen del swapchain.
		/// @pre Se ha llamado a IRenderer::Initialize.
		USize32 GetSwapchainImagesCount() const;

		/// @return Índidce del próximo fotograma que será presentado.
		/// @pre Se ha llamado a IRenderer::Initialize.
		virtual USize32 GetCurrentFrameIndex() const = 0;

		/// @return índidce de la lista de comandos que será usada en el próximo fotograma.
		/// @pre Se ha llamado a IRenderer::Initialize.
		virtual USize32 GetCurrentCommandListIndex() const = 0;

		/// @return Índidce de los recursos procesados en el fotograma actual.
		/// @pre Se ha llamado a IRenderer::Initialize.
		USize32 GetCurrentResourceIndex() const;


		/// @return API de renderizado de bajo nivel del renderizador actual.
		RenderApiType GetRenderApi() const;

#pragma endregion

#pragma region Image upload

		/// @brief Rellena la imagen en la GPU con los datos dados.
		/// @param destination Imagen a escribir.
		/// @param data Datos a escribir.
		/// @param numBytes Tamaño de los datos, en bytes.
		/// @param numLayers Número de capas a escribir.
		/// @param cmdList Lista de comandos usada para ejecutar la copia.
		/// 
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// 
		/// @pre @p cmdList debe estar empezada.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		void UploadLayeredImageToGpu(
			GpuImage* destination, 
			const TByte* data, 
			USize64 numBytes, 
			USize32 numLayers, 
			ICommandList* cmdList);

		/// @brief Rellena la imagen en la GPU con los datos dados.
		/// @param destination Imagen a escribir.
		/// @param data Datos a escribir.
		/// @param numBytes Tamaño de los datos, en bytes.
		/// @param cmdList Lista de comandos usada para ejecutar la copia.
		/// 
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// 
		/// @pre @p cmdList debe estar empezada.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		void UploadImageToGpu(
			GpuImage* destination, 
			const TByte* data, 
			USize64 numBytes,
			ICommandList* cmdList);

		/// @brief Rellena la imagen cubemap en la GPU con los datos dados.
		/// @param destination Imagen a escribir.
		/// @param data Datos a escribir.
		/// @param numBytes Tamaño de los datos, en bytes.
		/// @param cmdList Lista de comandos usada para ejecutar la copia.
		/// 
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// 
		/// @pre @p cmdList debe estar empezada.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		void UploadCubemapImageToGpu(
			GpuImage* destination, 
			const TByte* data, 
			USize64 numBytes,
			ICommandList* cmdList);

#pragma endregion

#pragma region Factory methods

		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre algún error durante la compilación de los shaders.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		virtual OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(
			const PipelineCreateInfo& pipelineInfo,
			const MaterialLayout& layout,
			const VertexInfo& vertexTypeName) = 0;

		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre algún error durante la compilación de los shaders.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		virtual OwnedPtr<IMeshPipeline> _CreateMeshPipeline(
			const PipelineCreateInfo& pipelineInfo,
			const MaterialLayout& layout) = 0;

		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre algún error durante la compilación de los shaders.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		virtual OwnedPtr<IRaytracingPipeline> _CreateRaytracingPipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout, 
			const VertexInfo& vertexTypeName) = 0;

		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre algún error durante la compilación de los shaders.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		virtual OwnedPtr<IComputePipeline> _CreateComputePipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout) = 0;

		/// @throws DescriptorPoolCreationException Si hay algún error nativo durante la creación del material slot.
		/// @throws DescriptorLayoutCreationException Si hay algún error nativo durante la creación del material slot.
		/// @throws MaterialSlotCreationException Si hay algún error durante la creación del material slot. 
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		virtual OwnedPtr<IMaterialSlot> _CreateMaterialSlot(
			const std::string& name, 
			const MaterialLayout& layout) const = 0;

		/// @return Crea una lista de comandos para un único uso.
		/// @param queueType Tipo de cola en la que se enviará.
		/// @param threadId ID del hilo que llama a la función.
		/// 
		/// @pre @p queueType debe ser distinto a `GpuQueueType::PRESENTATION`.
		/// @throws InvalidArgumentException si @p queueType es `GpuQueueType::PRESENTATION`.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @threadsafe
		OwnedPtr<ICommandList> CreateSingleUseCommandList(GpuQueueType queueType, std::thread::id threadId = std::this_thread::get_id());

		/// @param targetQueueType Cola de referencia, para la cual los comandos creados
		/// por el pool devuelto deben ser compatibles.
		/// @return Nueva cola de comandos.
		/// 
		/// @pre @p targetQueueType debe ser un puntero estable.
		/// 
		/// @threadsafe
		virtual OwnedPtr<ICommandPool> CreateCommandPool(const ICommandQueue* targetQueueType) = 0;

#pragma endregion

		/// @brief Ejecuta el contenido de la lista de comandos.
		/// @param commandList Lista de comandos a ejecutar.
		/// Su posesoón pasa a ser del renderizador.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @throws	CommandQueueSubmitException Si hay un error en la ejecución
		/// de la lista.
		/// 
		/// @note La lista de comandos será eliminada al finalizar el frame.
		/// @note Bloquea el hilo hasta que se haya completado la operación.
		/// 
		/// @threadsafe
		virtual void SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) = 0;

		const IGpuImageSampler& GetSampler(const GpuImageSamplerDesc& desc) const;

#pragma region Queues

		/// @return Cola de GPU unificada, para ejecutar todo tipo de comandos
		/// + operaciones de presentación.
		/// 
		/// @stablepointer
		/// 
		/// @pre `UseUnifiedCommandQueue()` debe devolver `true`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @threadsafe
		const ICommandQueue* GetUnifiedQueue() const;
		ICommandQueue* GetUnifiedQueue();

		/// @return Cola de GPU para la ejecución de comandos gráficos, de computación
		/// y de transferencia.
		/// 
		/// @stablepointer
		/// 
		/// @pre `UseUnifiedCommandQueue()` debe devolver `false`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @warning No soporta operaciones de presentación.
		/// 
		/// @threadsafe
		const ICommandQueue* GetGraphicsComputeQueue() const;
		ICommandQueue* GetGraphicsComputeQueue();

		/// @return Cola de GPU para ejecución de operaciones de presentación únicamente.
		/// 
		/// @stablepointer
		/// 
		/// @pre `UseUnifiedCommandQueue()` debe devolver `false`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @warning No soporta operaciones de gráficos.
		/// @warning No soporta operaciones de computación.
		/// @warning No soporta operaciones de transferencia.
		/// 
		/// @threadsafe
		const ICommandQueue* GetPresentationQueue() const;

		/// @return True si existe una cola de GPU unificada para
		/// todo tipo de operaciones.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @threadsafe
		bool UseUnifiedCommandQueue() const;

		/// @return Cola de GPU para ejecución de operaciones de transferencia únicamente.
		/// 
		/// @stablepointer
		/// 
		/// @pre `HasTransferOnlyQueue()` debe devolver `true`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @warning No soporta operaciones de gráficos.
		/// @warning No soporta operaciones de computación.
		/// @warning No soporta operaciones de presentación.
		/// 
		/// @threadsafe
		const ICommandQueue* GetTransferOnlyQueue() const;

		/// @return True si dispone de una cola de comandos exclusiva
		/// para operaciones de transferencia.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @threadsafe
		bool HasTransferOnlyQueue() const;


		/// @return Cola de comandos que sirve para el renderizado general.
		/// 
		/// @details La cola soportará, al menos, comandos gráficos, de computación
		/// y de transferencia.
		/// 
		/// @details Si @p `UseUnifiedCommandQueue()` es `true`, entonces
		/// devuelve `GetUnifiedQueue()`.
		/// @details Si @p `UseUnifiedCommandQueue()` es `false`, entonces
		/// devuelve `GetGraphicsComputeQueue()`.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @threadsafe
		const ICommandQueue* GetMainRenderingQueue() const;

		/// @param type Tipo de cola de GPU.
		/// @return Cola referenciada.
		/// 
		/// @threadsafe
		const ICommandQueue* GetOptimalQueue(GpuQueueType type) const;

#pragma endregion


		/// @return True si la funcionalidad de raytracing está activa.
		bool IsRtActive() const;

		/// @return True si el cambio de tamaño de la ventana se maneja implícitamente
		/// en el renderizador.
		bool _HasImplicitResizeHandling() const;


		/// @return Lista de comandos principal, sobre la cual grabar
		/// los comandos de renderizado.
		/// 
		/// Esta lista soporta comandos gráficos, de computación
		/// y de transferencia.
		/// 
		/// @stablepointer
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		ICommandList* GetMainCommandList();


		/// @return True si el renderizador ha sido inicializado,
		/// false en caso contrario.
		bool IsOpen() const;

#pragma region Fullscreen sprite rendering

		/// @return Material que se puede usar para
		/// renderizar imágenes en patnalla completa.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		Material* GetFullscreenRenderingMaterial();

#pragma endregion

	protected:

		/// @brief Inicializa las variables indicadas, 
		/// así como el sistema de materiales.
		/// @param renderApiType Tipo de API gráfica nativa.
		/// @param requestRayTracing Si debe cargar
		/// propiedades de trazado de rayos.
		IRenderer(
			RenderApiType renderApiType, 
			bool requestRayTracing) noexcept;

		/// @throws CommandPoolCreationException Si ocurre algún error.
		virtual void CreateCommandQueues() = 0;
		virtual void CreateSwapchain(PresentMode mode, const Vector2ui& resolution) = 0;
		virtual void CreateGpuMemoryAllocator() = 0;
		
		/// @brief Crea el renderpass que permite renderizar
		/// sobre las imágenes del swapchain.
		void CreateMainRenderpass();


		// --- Command pools --- //

		/// @return Pool de comandos válido para todo tipo de comandos.
		/// Las listas creadas con este pool deben ser mandadas a la
		/// cola unificada.
		/// 
		/// @param threadId ID del hilo que llama a la función.
		/// 
		/// @stablepointer
		/// @threadsafe
		/// 
		/// @pre `UseUnifiedCommandPool()` debe devolver `true`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		ICommandPool* GetUnifiedCommandPool(std::thread::id threadId);

		/// @return Pool de comandos válido para comandos gráficos, de computación
		/// y de transferencia.
		/// las listas creadas con este pool deben ser mandadas a la cola 
		/// `GetGraphicsComputeQueue()`.
		/// 
		/// @param threadId ID del hilo que llama a la función.
		/// 
		/// @stablepointer
		/// @threadsafe
		/// 
		/// @pre `UseUnifiedCommandPool()` debe devolver `false`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @warning No soporta operaciones de presentación.
		ICommandPool* GetGraphicsComputeCommandPool(std::thread::id threadId);

		/// @return True si existe un command pool unificado para
		/// todo tipo de comandos.
		/// 
		/// @threadsafe
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		bool UseUnifiedCommandPool() const;


		/// @return Pool de comandos exclusívamente de transferencia.
		/// 
		/// @pre `HasTransferOnlyCommandPool()` debe devolver `true`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @param threadId ID del hilo que llama a la función.
		/// 
		/// @stablepointer
		/// @threadsafe
		/// 
		/// @warning No soporta operaciones de gráficos.
		/// @warning No soporta operaciones de computación.
		/// @warning No soporta operaciones de transferencia.
		ICommandPool* GetTransferOnlyCommandPool(std::thread::id threadId);

		/// @return True si se posee una cola para transferencia de recursos.
		///
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @threadsafe
		bool HasTransferOnlyCommandPool() const;

		

		// Render target sobre el swapchian.
		UniquePtr<RenderTarget> m_finalRenderTarget;
		
		/// @brief Listas de comandos de un solo uso.
		DynamicArray<UniquePtr<ICommandList>> m_singleTimeCommandLists;

		bool m_isFirstRender = true;

		bool m_implicitResizeHandling = false;

		bool m_isRtActive = false;

		bool IsRtRequested() const;

		void RegisterUnifiedCommandPool(const ICommandQueue* unifiedQueue);
		void RegisterGraphicsCommputeCommandPool(const ICommandQueue* graphicsComputeQueue);
		void RegisterTransferOnlyCommandPool(const ICommandQueue* transferQueue);

		void _SetUnifiedCommandQueue(OwnedPtr<ICommandQueue> commandQueue);
		void _SetGraphicsCommputeCommandQueue(OwnedPtr<ICommandQueue> commandQueue);
		void _SetPresentationCommandQueue(OwnedPtr<ICommandQueue> commandQueue);
		void _SetTransferOnlyCommandQueue(OwnedPtr<ICommandQueue> commandQueue);

		void _SetMainCommandList(OwnedPtr<ICommandList> commandList);

		void _SetGpu(OwnedPtr<IGpu> gpu);
		void _SetMemoryAllocator(OwnedPtr<IGpuMemoryAllocator> allocator);
		void _SetSwapchain(OwnedPtr<ISwapchain> swapchain);


		// --- Sincronización --- //

		MutexHolder m_queueSubmitMutex;

	private:

		// --- Objetos singleton --- //

		UniquePtr<IGpu> m_currentGpu;
		UniquePtr<IGpuMemoryAllocator> m_gpuMemoryAllocator;
		UniquePtr<ISwapchain> m_swapchain;
		UniquePtr<MaterialSystem> m_materialSystem;

		ShaderPassFactory m_shaderPassFactory{};


		// --- Mapa de samplers --- //

		mutable std::unordered_map<GpuImageSamplerDesc, UniquePtr<IGpuImageSampler>> m_samplers;
		mutable std::shared_mutex m_samplerMapMutex;


		// --- Pools de comandos --- //

		/// @brief Usada si hay una cola capaz de soportar
		/// todos los tipos de comandos (gráficos, compute,
		/// transferencia y presentación).
		std::optional<ThreadedCommandPoolMap> m_unifiedCommandPools;

		/// @brief Usada si no hay una cola capaz de soportar
		/// todos los tipos de comandos.
		std::optional<ThreadedCommandPoolMap> m_graphicsComputeCommandPools;

		/// @brief Usada únicamente si existe una cola exclusiva de transferencia.
		std::optional<ThreadedCommandPoolMap> m_transferOnlyCommandPools;


		// --- Colas de comandos --- //

		UniquePtr<ICommandQueue> m_unifiedQueue;
		UniquePtr<ICommandQueue> m_graphicsComputeQueue;
		UniquePtr<ICommandQueue> m_presentationQueue;

		UniquePtr<ICommandQueue> m_transferOnlyQueue;


		// --- Listas de comandos --- //

		UniquePtr<ICommandList> m_mainCommandList;

		RenderApiType m_renderApiType;
		bool m_isRtRequested = false;

	};

}
