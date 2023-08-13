#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "DynamicArray.hpp"


#include "IGpu.h"
#include "ICommandQueue.h"
#include "ISwapchain.h"
#include "ICommandList.h"
#include "ICommandPool.h"
#include "IGpuMemoryAllocator.h"
#include "RenderTarget.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "PresentMode.h"

#include "IGpuImage.h"
#include "GpuBuffer.h"

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
	/// @note Esta interfaz no implementa ning�n c�digo, se debe usar una instancia de
	/// RendererVulkan o RendererDx12.
	/// </summary>
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

		/// <summary> Cierra el renderizador. </summary>
		/// 
		/// @note Tambi�n se llama en el destructor.
		virtual void Close() = 0;

		/// <summary> Reconfigura el swapchain al cambiar de tama�o la ventana. </summary>
		virtual void HandleResize();

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

		ICommandList* GetPreComputeCommandList();
		ICommandList* GetGraphicsCommandList();
		ICommandList* GetPostComputeCommandList();
		ICommandList* GetFrameBuildCommandList();

		inline ICommandQueue* GetGraphicsCommandQueue() { return graphicsQueue.GetPointer(); }
		inline ICommandQueue* GetComputeCommandQueue() { return computeQueue.GetPointer(); }
		inline ICommandQueue* GetPresentCommandQueue() { return presentQueue.GetPointer(); }

		/// <summary> Devuelve un puntero al asignador de memoria de la GPU. </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @note Es un puntero estable.
		/// @warning Ser� nulo si no se ha llamado a IRenderer::Initialize.
		IGpuMemoryAllocator* GetAllocator();

		/// <summary> Devuelve un puntero al objeto Gpu. </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @note Es un puntero estable.
		/// @warning Ser� nulo si no se ha llamado a IRenderer::Initialize.
		IGpu* GetGpu();

		/// <summary> Devuelve un puntero al sistema de materiales. </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @note Es un puntero estable.
		/// @warning Ser� nulo si no se ha llamado a IRenderer::Initialize.
		MaterialSystem* GetMaterialSystem();

		/// <summary> Devuelve el swapchain de la aplicaci�n. </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @note Es un puntero estable.
		/// @warning Ser� nulo si no se ha llamado a IRenderer::Initialize.
		/// @warning Funci�n interna, no usar.
		ISwapchain* _GetSwapchain();


		/// <summary> Devuelve el render target que renderiza sobre la imagen final del swapchain. </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		/// @note Es un puntero estable.
		/// @warning Ser� nulo si no se ha llamado a IRenderer::Initialize.
		RenderTarget* GetFinalRenderTarget();

		/// <summary>
		/// Devuelve una c�mara 2D que renderiza en la resoluci�n
		/// completa de la pantalla.
		/// 
		/// Para poder renderizar con facilidad los diferentes render targets
		/// en los sistemas de renderizado y en IGame::BuildFrame.
		/// </summary>
		/// 
		/// @pre Se ha llamado a IRenderer::Initialize.
		const ECS::CameraComponent2D& GetRenderTargetsCamera() const;


		/// <summary> Devuelve true si el renderizador est� inicializado y funcionando. </summary>
		bool IsOpen() const;

		/// <summary>
		/// True si el renderizador soporta trazado de rayos.
		/// Depende de la GPU usada.
		/// </summary>
		virtual bool SupportsRaytracing() const = 0;


		/// <summary>
		/// Devuelve el n�mero de im�genes del swapchain.
		/// Para recursos que necesiten tener una copia por imagen del swapchain.
		/// </summary>
		USize32 GetSwapchainImagesCount() const;

		/// <summary> Devuelve el �ndidce del pr�ximo fotograma que ser� presentado. </summary>
		virtual USize32 GetCurrentFrameIndex() const = 0;

		/// <summary> Devuelve el �ndidce de la lista de comandos que ser� usada en el prximo fotograma. </summary>
		virtual USize32 GetCurrentCommandListIndex() const = 0;

		/// <summary> Devuelve el �ndidce de los recursos procesados en el fotograma actual. </summary>
		USize32 GetCurrentResourceIndex() const;


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
			USize64 numBytes, 
			USize32 numLayers, 
			ICommandList* cmdList);

		/// <summary> Rellena la imagen en la GPU con los datos dados. </summary>
		/// 
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		void UploadImageToGpu(
			GpuImage* destination, 
			const TByte* data, 
			USize64 numBytes,
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
			USize64 numBytes,
			ICommandList* cmdList);

#pragma endregion

#pragma region Factory methods

		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
		virtual OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout, 
			const VertexInfo& vertexTypeName) = 0;

		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
		virtual OwnedPtr<IRaytracingPipeline> _CreateRaytracingPipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout, 
			const VertexInfo& vertexTypeName) = 0;

		/// @throws FileNotFoundException si no se encuentra los archivo de shader necesarios.
		/// @throws ShaderLoadingException si no se consigue cargar / compilar los shaders.
		/// @throws PipelineCreationException si no se consigue crear el pipeline.
		/// @throws ShaderCompilingException Si ocurre alg�n error durante la compilaci�n de los shaders.
		virtual OwnedPtr<IComputePipeline> _CreateComputePipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout) = 0;

		/// @throws DescriptorPoolCreationException Si hay alg�n error nativo durante la creaci�n del material slot.
		/// @throws DescriptorLayoutCreationException Si hay alg�n error nativo durante la creaci�n del material slot.
		/// @throws MaterialSlotCreationException Si hay alg�n error durante la creaci�n del material slot. 
		virtual OwnedPtr<IMaterialSlot> _CreateMaterialSlot(
			const std::string& name, 
			const MaterialLayout& layout) const = 0;

		/// <summary> Crea una lista de comandos para un �nico uso. </summary>
		OwnedPtr<ICommandList> CreateSingleUseCommandList();
		/// <summary> Ejecuta el contenido de la lista de comandos. </summary>
		
		/// @brief Ejecuta el contenido de la lista de comandos.
		/// @param commandList Lista de comandos a ejecutar.
		/// Su poseso�n pasa a ser del renderizador.
		/// 
		/// @throws	CommandQueueSubmitException Si hay un error en la ejecuci�n
		/// de la lista.
		/// 
		/// @note La lista de comandos ser� eliminada al finalizar el frame.
		virtual void SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) = 0;

#pragma endregion


		/// <summary>
		/// True si la funcionalidad de raytracing est� activa.
		/// </summary>
		bool IsRtActive() const;

		/// <summary>
		/// True si el cambio de tama�o de la ventana se maneja impl�citamente
		/// en el renderizador.
		/// </summary>
		bool _HasImplicitResizeHandling() const;


		Material* GetFullscreenRenderingMaterial();

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
		/// @warning Si se sobreescribe, devolver� un puntero distinto al original, el cual deber� ser
		/// eliminado tras llamar a esta funci�n.</returns>
		virtual const TByte* FormatImageDataForGpu(const GpuImage* image, const TByte* data, USize32 numLayers);

		/// @throws CommandPoolCreationException Si ocurre alg�n error.
		virtual void CreateCommandQueues() = 0;
		virtual void CreateSwapchain(PresentMode mode) = 0;
		virtual void CreateGpuMemoryAllocator() = 0;
		
		void CreateMainRenderpass();

		UniquePtr<IGpu> currentGpu;

		UniquePtr<ICommandQueue> graphicsQueue;
		UniquePtr<ICommandQueue> computeQueue;
		UniquePtr<ICommandQueue> presentQueue;

		UniquePtr<ISwapchain> swapchain;

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

		bool isFirstRender = true;

		const IO::IDisplay* display = nullptr;

		bool isOpen = false;

		UniquePtr<ECS::CameraComponent2D> renderTargetsCamera;
		ECS::Transform2D renderTargetsCameraTransform{ ECS::EMPTY_GAME_OBJECT };

		bool implicitResizeHandling = false;

		bool isRtActive = false;

		bool IsRtRequested() const;

		// True si se usa la misma queue para enviar comandos gr�ficos y de computaci�n.
		bool singleCommandQueue = false;

	private:

		RenderApiType renderApiType;
		bool isRtRequested = false;

	};

}
