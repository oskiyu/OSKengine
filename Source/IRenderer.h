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

// Sincronizaci�n.
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
		/// @param appName Nombre de la aplicaci�n / juego.
		/// @param version Versi�n de la aplicaci�n / juego.
		/// @param display Ventana enlazada (sobre la que se renderizar� el juego).
		/// @param mode Modo de sincronizaci�n vertical.
		/// 
		/// @throws GpuNotFoundException Si no se encuentra ninguna GPU compatible.
		/// @throws LogicalDeviceCreationException Si ocurre alg�n error al establecer la conexi�n l�gica con la GPU elegida.
		/// @throws CommandPoolCreationException Si ocurre alg�n error al crear las pools de comandos.
		/// @throws RendererCreationException Si ocurre alg�n otro error.
		virtual void Initialize(
			const std::string& appName, 
			const Version& version, 
			const IO::IDisplay& display, 
			PresentMode mode) = 0;

		/// @brief Cierra el renderizador.
		/// @note Tambi�n se llama en el destructor.
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


		/// @brief Cambia el modo de presentaci�n del renderizador.
		/// @param mode Modo de sincronizaci�n vertical.
		void SetPresentMode(PresentMode mode);

		/// @brief Reconfigura el swapchain al cambiar de tama�o la ventana.
		/// @param resolution Nuevo tama�o de la imagen.
		virtual void HandleResize(const Vector2ui& resolution);

		/// @brief Una vez se han grabado todos los comandos, se debe iniciar su
		/// ejecuci�n en la GPU para ser renderizados.
		/// 
		/// @throws CommandListSubmitException Si ocurre alg�n error al mandar las listas.
		/// @throws CommandQueueSubmitException Si ocurre alg�n error al ejecutar las colas.
		virtual void PresentFrame() = 0;

		/// @brief Hace que el hilo actual espere a que se hayan ejecutado
		/// todos los comandos en la GPU.
		/// Necesario si se van a eliminar recursos que puede que todav�a
		/// est�n en uso.
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

		/// @return Swapchain de la aplicaci�n.
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @stablepointer
		/// @warning Funci�n interna, no usar.
		ISwapchain* _GetSwapchain();


		/// @return Devuelve el render target que renderiza sobre la imagen final del swapchain.
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @stablepointer
		RenderTarget* GetFinalRenderTarget();


		/// @return True si el renderizador soporta trazado de rayos.
		/// Depende de la GPU usada.
		/// @pre Se ha llamado a IRenderer::Initialize.
		virtual bool SupportsRaytracing() const = 0;


		/// @return Devuelve el n�mero de im�genes del swapchain.
		/// Para recursos que necesiten tener una copia por imagen del swapchain.
		/// @pre Se ha llamado a IRenderer::Initialize.
		USize32 GetSwapchainImagesCount() const;

		/// @return �ndidce del pr�ximo fotograma que ser� presentado.
		/// @pre Se ha llamado a IRenderer::Initialize.
		virtual USize32 GetCurrentFrameIndex() const = 0;

		/// @return �ndidce de la lista de comandos que ser� usada en el pr�ximo fotograma.
		/// @pre Se ha llamado a IRenderer::Initialize.
		virtual USize32 GetCurrentCommandListIndex() const = 0;

		/// @return �ndidce de los recursos procesados en el fotograma actual.
		/// @pre Se ha llamado a IRenderer::Initialize.
		USize32 GetCurrentResourceIndex() const;


		/// @return API de renderizado de bajo nivel del renderizador actual.
		RenderApiType GetRenderApi() const;

#pragma endregion

#pragma region Image upload

		/// @brief Rellena la imagen en la GPU con los datos dados.
		/// @param destination Imagen a escribir.
		/// @param data Datos a escribir.
		/// @param numBytes Tama�o de los datos, en bytes.
		/// @param numLayers N�mero de capas a escribir.
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
		/// @param numBytes Tama�o de los datos, en bytes.
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
		/// @param numBytes Tama�o de los datos, en bytes.
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
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
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
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		virtual OwnedPtr<IMeshPipeline> _CreateMeshPipeline(
			const PipelineCreateInfo& pipelineInfo,
			const MaterialLayout& layout) = 0;

		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
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
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		virtual OwnedPtr<IComputePipeline> _CreateComputePipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout) = 0;

		/// @throws DescriptorPoolCreationException Si hay alg�n error nativo durante la creaci�n del material slot.
		/// @throws DescriptorLayoutCreationException Si hay alg�n error nativo durante la creaci�n del material slot.
		/// @throws MaterialSlotCreationException Si hay alg�n error durante la creaci�n del material slot. 
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		virtual OwnedPtr<IMaterialSlot> _CreateMaterialSlot(
			const std::string& name, 
			const MaterialLayout& layout) const = 0;

		/// @return Crea una lista de comandos para un �nico uso.
		/// @param queueType Tipo de cola en la que se enviar�.
		/// @param threadId ID del hilo que llama a la funci�n.
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
		/// Su poseso�n pasa a ser del renderizador.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @throws	CommandQueueSubmitException Si hay un error en la ejecuci�n
		/// de la lista.
		/// 
		/// @note La lista de comandos ser� eliminada al finalizar el frame.
		/// @note Bloquea el hilo hasta que se haya completado la operaci�n.
		/// 
		/// @threadsafe
		virtual void SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) = 0;

		const IGpuImageSampler& GetSampler(const GpuImageSamplerDesc& desc) const;

#pragma region Queues

		/// @return Cola de GPU unificada, para ejecutar todo tipo de comandos
		/// + operaciones de presentaci�n.
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

		/// @return Cola de GPU para la ejecuci�n de comandos gr�ficos, de computaci�n
		/// y de transferencia.
		/// 
		/// @stablepointer
		/// 
		/// @pre `UseUnifiedCommandQueue()` debe devolver `false`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @warning No soporta operaciones de presentaci�n.
		/// 
		/// @threadsafe
		const ICommandQueue* GetGraphicsComputeQueue() const;
		ICommandQueue* GetGraphicsComputeQueue();

		/// @return Cola de GPU para ejecuci�n de operaciones de presentaci�n �nicamente.
		/// 
		/// @stablepointer
		/// 
		/// @pre `UseUnifiedCommandQueue()` debe devolver `false`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @warning No soporta operaciones de gr�ficos.
		/// @warning No soporta operaciones de computaci�n.
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

		/// @return Cola de GPU para ejecuci�n de operaciones de transferencia �nicamente.
		/// 
		/// @stablepointer
		/// 
		/// @pre `HasTransferOnlyQueue()` debe devolver `true`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @warning No soporta operaciones de gr�ficos.
		/// @warning No soporta operaciones de computaci�n.
		/// @warning No soporta operaciones de presentaci�n.
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
		/// @details La cola soportar�, al menos, comandos gr�ficos, de computaci�n
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


		/// @return True si la funcionalidad de raytracing est� activa.
		bool IsRtActive() const;

		/// @return True si el cambio de tama�o de la ventana se maneja impl�citamente
		/// en el renderizador.
		bool _HasImplicitResizeHandling() const;


		/// @return Lista de comandos principal, sobre la cual grabar
		/// los comandos de renderizado.
		/// 
		/// Esta lista soporta comandos gr�ficos, de computaci�n
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
		/// renderizar im�genes en patnalla completa.
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		Material* GetFullscreenRenderingMaterial();

#pragma endregion

	protected:

		/// @brief Inicializa las variables indicadas, 
		/// as� como el sistema de materiales.
		/// @param renderApiType Tipo de API gr�fica nativa.
		/// @param requestRayTracing Si debe cargar
		/// propiedades de trazado de rayos.
		IRenderer(
			RenderApiType renderApiType, 
			bool requestRayTracing) noexcept;

		/// @throws CommandPoolCreationException Si ocurre alg�n error.
		virtual void CreateCommandQueues() = 0;
		virtual void CreateSwapchain(PresentMode mode, const Vector2ui& resolution) = 0;
		virtual void CreateGpuMemoryAllocator() = 0;
		
		/// @brief Crea el renderpass que permite renderizar
		/// sobre las im�genes del swapchain.
		void CreateMainRenderpass();


		// --- Command pools --- //

		/// @return Pool de comandos v�lido para todo tipo de comandos.
		/// Las listas creadas con este pool deben ser mandadas a la
		/// cola unificada.
		/// 
		/// @param threadId ID del hilo que llama a la funci�n.
		/// 
		/// @stablepointer
		/// @threadsafe
		/// 
		/// @pre `UseUnifiedCommandPool()` debe devolver `true`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		ICommandPool* GetUnifiedCommandPool(std::thread::id threadId);

		/// @return Pool de comandos v�lido para comandos gr�ficos, de computaci�n
		/// y de transferencia.
		/// las listas creadas con este pool deben ser mandadas a la cola 
		/// `GetGraphicsComputeQueue()`.
		/// 
		/// @param threadId ID del hilo que llama a la funci�n.
		/// 
		/// @stablepointer
		/// @threadsafe
		/// 
		/// @pre `UseUnifiedCommandPool()` debe devolver `false`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @warning No soporta operaciones de presentaci�n.
		ICommandPool* GetGraphicsComputeCommandPool(std::thread::id threadId);

		/// @return True si existe un command pool unificado para
		/// todo tipo de comandos.
		/// 
		/// @threadsafe
		/// 
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		bool UseUnifiedCommandPool() const;


		/// @return Pool de comandos exclus�vamente de transferencia.
		/// 
		/// @pre `HasTransferOnlyCommandPool()` debe devolver `true`.
		/// @pre El renderizador debe haber sido previamente inicializado.
		/// @pre El renderizador no debe haber sido previamente cerrado.
		/// 
		/// @param threadId ID del hilo que llama a la funci�n.
		/// 
		/// @stablepointer
		/// @threadsafe
		/// 
		/// @warning No soporta operaciones de gr�ficos.
		/// @warning No soporta operaciones de computaci�n.
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


		// --- Sincronizaci�n --- //

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
		/// todos los tipos de comandos (gr�ficos, compute,
		/// transferencia y presentaci�n).
		std::optional<ThreadedCommandPoolMap> m_unifiedCommandPools;

		/// @brief Usada si no hay una cola capaz de soportar
		/// todos los tipos de comandos.
		std::optional<ThreadedCommandPoolMap> m_graphicsComputeCommandPools;

		/// @brief Usada �nicamente si existe una cola exclusiva de transferencia.
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
