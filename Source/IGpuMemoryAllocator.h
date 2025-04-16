#pragma once

#include "ApiCall.h"
#include "UniquePtr.hpp"
#include "HashMap.hpp"
#include "DynamicArray.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"

// Recursos
#include "IGpuImage.h"
#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"

// Ray-tracing
#include "ITopLevelAccelerationStructure.h"
#include "IBottomLevelAccelerationStructure.h"

// Para `TIndexSize`
#include "Vertex.h"

// Sincronización.
#include "MutexHolder.h"

// Otros.
#include "GpuMemoryUsageInfo.h"


namespace OSK::GRAPHICS {

	// Enumeraciones.
	enum class GpuBufferUsage;
	enum class GpuQueueType;

	// Recursos.
	class GpuBuffer;
	class GpuImage;
	struct GpuImageCreateInfo;

	// Vértices.
	struct Vertex3D;
	class VertexInfo;

	struct VertexBufferView;
	struct IndexBufferView;

	// Interfaces.
	class IGpu;


	struct OSKAPI_CALL GpuBufferMemoryBlockInfo {
		USize64 size;
		GpuBufferUsage usage;
		GpuSharedMemoryType sharedType;

		bool operator==(const GpuBufferMemoryBlockInfo& other) const;
	};
	
	constexpr static USize32 GPU_MEMORY_NO_ALIGNMENT = 1;

	/// @brief El asignador de memoria se encarga de reservar grandes bloques
	/// de memoria en la GPU, que después podrán ser usados para
	/// almacenar texturas, buffers, etc.
	/// 
	/// La memoria de un bloque se divide en subbloques. Al reservar
	/// un recurso, se crea un subbloque que ocupará parte de la
	/// memoria del bloque.
	/// 
	/// @threadsafe
	class OSKAPI_CALL IGpuMemoryAllocator {

	public:

		explicit IGpuMemoryAllocator(IGpu* device);
		virtual ~IGpuMemoryAllocator();

		/// <summary>
		/// El tamaño de memoria que, por defecto, tendrá cada bloque de memoria.
		/// 
		/// @note Un bloque puede tener más tamaño, si se usa para almacenar un
		/// recurso que ocupe más espacio.
		/// </summary>
		static const USize64 SizeOfMemoryBlockInMb;

		OSK_DEFINE_AS(IGpuMemoryAllocator);
		OSK_DISABLE_COPY(IGpuMemoryAllocator);

		/// @brief Obtiene la información sobre el uso de memoria de esta GPU,
		/// incluyendo espacio usado y espacio disponible.
		/// @return Información sobre el uso de memoria de esta GPU.
		/// 
		/// @threadsafe
		GpuMemoryUsageInfo GetMemoryUsageInfo() const;
		
		/// @brief Crea una nueva imagen en la GPU.
		/// @param info Información para la creación de la imagen.
		/// 
		/// @threadsafe
		virtual UniquePtr<GpuImage> CreateImage(const GpuImageCreateInfo& info) = 0;

		/// @brief Crea una imagen cubemap sin inicializar en la GPU.
		/// @param faceSize Resolución de cada cara, en píxeles.
		/// @param format Formato de todas las caras.
		/// @param usage Uso que se le va a dar al cubemap.
		/// @param sharedType Tipo de memoria preferente.
		/// @param queueType Tipo de cola sobre la que se alojará.
		/// @param samplerDesc Sampler del cubemap.
		/// @return Imagen sin información.
		/// 
		/// @post La imagen devuelta tendrá al menos el uso GpuImageUsage::CUBEMAP.
		/// 
		/// @threadsafe
		UniquePtr<GpuImage> CreateCubemapImage(
			const Vector2ui& faceSize, 
			Format format, 
			GpuImageUsage usage, 
			GpuSharedMemoryType sharedType,
			GpuQueueType queueType,
			GpuImageSamplerDesc samplerDesc = {});

#pragma region Vertex buffers

		/// @brief Crea un buffer de vértices con los vértices dados.
		/// 
		/// @param data Puntero al array de vértices.
		/// @param vertexSize Tamaño de cada vértice individual, en bytes.
		/// @param numVertices Número de vértices pasados.
		/// @param vertexInfo Información del tipo de vértice.
		/// @param queueType Tipo de cola en la que se almacenará.
		/// @param usage Uso que se le dará al buffer, además de como vertex buffer.
		/// 
		/// @return Buffer en la GPU con los vértices.
		/// 
		/// @note El buffer se colocará preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendrá al menos los usos GpuBufferUsage::VERTEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		/// 
		/// @threadsafe
		UniquePtr<GpuBuffer> CreateVertexBuffer(
			const void* data,
			USize32 vertexSize,
			USize32 numVertices,
			const VertexInfo& vertexInfo,
			GpuQueueType queueType,
			GpuBufferUsage usage = GpuBufferUsage::VERTEX_BUFFER);

		/// @brief Crea un buffer de vértices con los vértices dados.
		/// 
		/// @param data Puntero al array de vértices.
		/// @param vertexSize Tamaño de cada vértice individual, en bytes.
		/// @param numVertices Número de vértices pasados.
		/// @param vertexInfo Información del tipo de vértice.
		/// @param transferQueue Tipo de cola que se usará para transferir el contenido.
		/// @param finalQueue Tipo de cola en la que se almacenará.
		/// @param usage Uso que se le dará al buffer, además de como vertex buffer.
		/// 
		/// @return Buffer en la GPU con los vértices.
		/// 
		/// @note El buffer se colocará preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendrá al menos los usos GpuBufferUsage::VERTEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		/// 
		/// @threadsafe
		UniquePtr<GpuBuffer> CreateVertexBuffer(
			const void* data,
			USize32 vertexSize,
			USize32 numVertices,
			const VertexInfo& vertexInfo,
			GpuQueueType transferQueue,
			GpuQueueType finalQueue,
			GpuBufferUsage usage = GpuBufferUsage::VERTEX_BUFFER);

		/// @brief Crea un buffer de vértices con los vértices dados.
		/// 
		/// @tparam T Tipo del vértice.
		/// @param vertices Lista con los vértices a añadir.
		/// @param vertexInfo Información del tipo del vértice.
		/// @param queueType Tipo de cola en la que se almacenará.
		/// @param usage Uso que se le dará al buffer, además de como vertex buffer.
		/// 
		/// @return Buffer en la GPU con los vértices.
		/// 
		/// @note El buffer se colocará preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendrá al menos los usos GpuBufferUsage::VERTEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		/// 
		/// @threadsafe
		template <typename T>
		inline UniquePtr<GpuBuffer> CreateVertexBuffer(
			const DynamicArray<T>& vertices,
			const VertexInfo& vertexInfo,
			GpuQueueType queueType,
			GpuBufferUsage usage = GpuBufferUsage::VERTEX_BUFFER) {
			return this->CreateVertexBuffer(vertices.GetData(), sizeof(T), vertices.GetSize(), vertexInfo, queueType, usage);
		}

		/// @brief Crea un buffer de vértices con los vértices dados.
		/// 
		/// @tparam T Tipo del vértice.
		/// @param vertices Lista con los vértices a añadir.
		/// @param vertexInfo Información del tipo del vértice.
		/// @param transferQueue Tipo de cola que se usará para transferir el contenido.
		/// @param finalQueue Tipo de cola en la que se almacenará.
		/// @param usage Uso que se le dará al buffer, además de como vertex buffer.
		/// 
		/// @return Buffer en la GPU con los vértices.
		/// 
		/// @note El buffer se colocará preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendrá al menos los usos GpuBufferUsage::VERTEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		/// 
		/// @threadsafe
		template <typename T>
		inline UniquePtr<GpuBuffer> CreateVertexBuffer(
			const DynamicArray<T>& vertices,
			const VertexInfo& vertexInfo,
			GpuQueueType transferQueue,
			GpuQueueType finalQueue,
			GpuBufferUsage usage = GpuBufferUsage::VERTEX_BUFFER) {
			return this->CreateVertexBuffer(vertices.GetData(), sizeof(T), vertices.GetSize(), vertexInfo, transferQueue, finalQueue, usage);
		}

#pragma endregion

#pragma region Index buffers

		/// @brief Crea un buffer de índices con los índices dados.
		/// @param indices Lista de índices.
		/// @param queueType Tipo de cola en la que se almacenará.
		/// @param usage Uso que se le dará al buffer, además de como index buffer.
		/// @return Buffer con los índices.
		/// 
		/// @note El buffer se colocará preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendrá al menos los usos GpuBufferUsage::INDEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		/// 
		/// @threadsafe
		UniquePtr<GpuBuffer> CreateIndexBuffer(
			const DynamicArray<TIndexSize>& indices,
			GpuQueueType queueType,
			GpuBufferUsage usage = GpuBufferUsage::INDEX_BUFFER);

		/// @brief Crea un buffer de índices con los índices dados.
		/// @param indices Lista de índices.
		/// @param queueTtransferQueueype Tipo de cola que se usará para la transferencia.
		/// @param queueType Tipo de cola en la que se almacenará.
		/// @param usage Uso que se le dará al buffer, además de como index buffer.
		/// @return Buffer con los índices.
		/// 
		/// @note El buffer se colocará preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendrá al menos los usos GpuBufferUsage::INDEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		/// 
		/// @threadsafe
		UniquePtr<GpuBuffer> CreateIndexBuffer(
			const DynamicArray<TIndexSize>& indices,
			GpuQueueType transferQueue,
			GpuQueueType queueType,
			GpuBufferUsage usage = GpuBufferUsage::INDEX_BUFFER);

#pragma endregion

		/// @brief Crea un uniform buffer.
		/// @param size Tamaño del uniform buffer.
		/// @param queueType Tipo de cola en la que se almacenará.
		/// @param usage Uso que se le dará al buffer, además de como uniform buffer.
		/// @return Uniform buffer.
		/// 
		/// @note El buffer se colocará en un bloque con GpuSharedMemoryType::GPU_AND_CPU.
		/// @post El buffer siempre tendrá al menos el uso GpuBufferUsage::UNIFORM_BUFFER.
		/// 
		/// @threadsafe
		UniquePtr<GpuBuffer> CreateUniformBuffer(
			USize64 size,
			GpuQueueType queueType = GpuQueueType::MAIN,
			GpuBufferUsage usage = GpuBufferUsage::UNIFORM_BUFFER);

		/// @brief Crea un storage buffer.
		/// @param size Tamaño, en bytes.
		/// @param queueType Tipo de cola en la que se almacenará.
		/// @param usage Uso que se le dará al buffer, además de como storage buffer.
		/// @return Storage buffer.
		/// 
		/// @note El buffer se colocará en un bloque con GpuSharedMemoryType::GPU_AND_CPU.
		/// @post El buffer siempre tendrá al menos el uso GpuBufferUsage::STORAGE_BUFFER.
		/// 
		/// @threadsafe
		UniquePtr<GpuBuffer> CreateStorageBuffer(
			USize64 size,
			GpuQueueType queueType,
			GpuBufferUsage usage = GpuBufferUsage::STORAGE_BUFFER);

		/// @brief Crea un buffer temporal con el tamaño dado.
		/// 
		/// Este buffer alojará memoria en una región donde la CPU tiene acceso,
		/// de tal manera que se podrán enviar datos a esta región, y después
		/// la GPU podrá copiar esta región de memoria a una región de memoria de video
		/// más rápida.
		/// @param size Tamaño del buffer, en bytes.
		/// @param queueType Tipo de cola en la que se almacenará.
		/// @param usage Uso que se le va a dar al buffer, además de como staging buffer.
		/// @return Staging buffer.
		/// 
		/// @note El buffer se colocará en un bloque con GpuSharedMemoryType::GPU_AND_CPU.
		/// @post El buffer siempre tendrá al menos los usos GpuBufferUsage::TRANSFER_SOURCE | GpuBufferUsage::UPLOAD_ONLY.
		/// 
		/// @threadsafe
		UniquePtr<GpuBuffer> CreateStagingBuffer(
			USize64 size,
			GpuQueueType queueType,
			GpuBufferUsage usage = GpuBufferUsage::UPLOAD_ONLY);

		/// @brief Crea un buffer genérico de memoria en la GPU con las características dadas.
		/// @param size Tamaño del buffer, en bytes.
		/// @param alignment Alineamiento del buffer respecto al inicio del bloque.
		/// @param usage Uso que se le dará al buffer.
		/// @param memoryType Tipo de memoria que se alojará.
		/// @param queueType Tipo de cola en la que se almacenará.
		/// @return Buffer en la GPU.
		/// 
		/// @threadsafe
		UniquePtr<GpuBuffer> CreateBuffer(
			USize64 size,
			USize64 alignment,
			GpuBufferUsage usage, 
			GpuSharedMemoryType memoryType,
			GpuQueueType queueType);


		/// @brief Crea una estructura de aceleración espacial para el trazado de rayos.
		/// 
		/// La estructura será de bajo nivel y contendrá la geometría definida
		/// por los vértices e índices dados.
		/// @param vertexBuffer Vértices del modelo 3D a partir del que se va a crear la estructura.
		/// @param indexBuffer Índices del modelo 3D a partir del que se va a crear la estructura.
		/// @return Estructura de bajo nivel.
		/// 
		/// @threadsafe
		UniquePtr<IBottomLevelAccelerationStructure> CreateBottomAccelerationStructure(
			const GpuBuffer& vertexBuffer,
			const GpuBuffer& indexBuffer);

		/// @brief Crea una estructura de aceleración espacial para el trazado de rayos.
		/// 
		/// La estructura será de bajo nivel y contendrá la geometría definida
		/// por los vértices e índices dados.
		/// @param vertexBuffer Vértices del modelo 3D a partir del que se va a crear la estructura.
		/// @param vertexView View de los vértices.
		/// @param indexBuffer Índices del modelo 3D a partir del que se va a crear la estructura.
		/// @param indexView View de los índices.
		/// @return Estructura de bajo nivel.
		/// 
		/// @threadsafe
		UniquePtr<IBottomLevelAccelerationStructure> CreateBottomAccelerationStructure(
			const GpuBuffer& vertexBuffer,
			const VertexBufferView& vertexView,
			const GpuBuffer& indexBuffer,
			const IndexBufferView& indexView);

		/// @brief Crea una estructura de aceleración espacial para el trazado de rayos.
		/// 
		/// La estructura agrupará varias estructuras de bajo nivel.
		/// @param bottomStructures Lista con todas las estructuras de bajo nivel a partir
		/// de los que se va a crear la estructura de alto nivel.
		/// @return Estructura de alto nivel.
		/// 
		/// @threadsafe
		UniquePtr<ITopLevelAccelerationStructure> CreateTopAccelerationStructure(
			DynamicArray<IBottomLevelAccelerationStructure*> bottomStructures);


		/// <summary>
		/// Quita uno de los bloques, que ha sido eliminado.
		/// </summary>
		/// 
		/// @threadsafe
		void RemoveMemoryBlock(IGpuMemoryBlock* block);

		/// @brief Libera toda la memoria usada para subir recursos.
		void FreeStagingMemory();


		/// @threadsafe
		const GpuImage* GetDefaultNormalTexture() const;

		/// @threadsafe
		USize64 GetAlignment(USize64 originalAlignment, GpuBufferUsage usage) const;

	protected:

		void LoadDefaultNormalTexture();

		//
		
		
		virtual UniquePtr<IBottomLevelAccelerationStructure> _CreateBottomAccelerationStructure() = 0;


		virtual UniquePtr<ITopLevelAccelerationStructure> _CreateTopAccelerationStructure() = 0;


		virtual UniquePtr<IGpuMemoryBlock> CreateNewBufferBlock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;
		virtual UniquePtr<IGpuMemoryBlock> CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) = 0;

		/// @brief Devuelve un bloque con las características dadas.
		/// 
		/// @param size Tamaño míniimo del bloque, en bytes.
		/// @param usage Uso que se le dará a la memoria.
		/// @param sharedType Tipo de memoria.
		/// @return Bloque con, al menos, el tamaño dado.
		/// 
		/// @remark Es posible que el tamaño del bloque sea mayor al indicado en size.
		/// @remark Si no hay ningún bloque con tamaño suficiente, se crea uno nuevo.
		/// 
		/// @threadsafe
		IGpuMemoryBlock* GetNextBufferMemoryBlock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// @brief Devuelve un subbloque con las características dadas.
		/// @param size Tamaño del subbloque, en bytes.
		/// @param usage Uso que se le dará a la memoria.
		/// @param sharedType Tipo de memoria.
		/// @return Bloque con el tamaño dado.
		/// 
		/// @threadsafe
		UniquePtr<IGpuMemorySubblock> GetNextBufferMemorySubblock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// @brief Información de los bloques.
		DynamicArray<GpuBufferMemoryBlockInfo> bufferMemoryBlocksInfo;
		DynamicArray<DynamicArray<UniquePtr<IGpuMemoryBlock>>> bufferMemoryBlocks;

		// Se almacenan en las propias imágenes.
		DynamicArray<IGpuMemoryBlock*> imageMemoryBlocks;

		UniquePtr<GpuImage> m_defaultNormalTexture;

		IGpu* device = nullptr;

		// Mutex para las alojaciones de memoria.
		// Para la búsqueda de bloques.
		MutexHolder m_memoryAllocationMutex;

	};

}
