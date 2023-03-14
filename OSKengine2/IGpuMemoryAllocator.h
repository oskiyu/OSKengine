#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "LinkedList.hpp"
#include "HashMap.hpp"
#include "DynamicArray.hpp"
#include "Vertex.h"
#include "Vector2.hpp"
#include "Vector3.hpp"

#include "GpuImageSamplerDesc.h"
#include "GpuMemoryTypes.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "GpuImageLayout.h"
#include "Format.h"

namespace OSK::GRAPHICS {

	class IGpuMemoryBlock;
	class IGpuMemorySubblock;
	class IGpu;
	enum class GpuBufferUsage;

	class IBottomLevelAccelerationStructure;
	class ITopLevelAccelerationStructure;
	class GpuDataBuffer;
	class GpuImage;
	class IGpuVertexBuffer;
	class IGpuIndexBuffer;
	class IGpuUniformBuffer;
	class IGpuStorageBuffer;
	struct Vertex3D;
	class VertexInfo;

	struct OSKAPI_CALL GpuBufferMemoryBlockInfo {
		unsigned int size;
		GpuBufferUsage usage;
		GpuSharedMemoryType sharedType;

		bool operator==(const GpuBufferMemoryBlockInfo& other) const;
	};

	struct GpuImageCreateInfo {

		static GpuImageCreateInfo CreateDefault1D(uint32_t resolution, Format format, GpuImageUsage usage);
		static GpuImageCreateInfo CreateDefault2D(const Vector2ui& resolution, Format format, GpuImageUsage usage);
		static GpuImageCreateInfo CreateDefault3D(const Vector3ui& resolution, Format format, GpuImageUsage usage);

		void SetMsaaSamples(TSize msaaSamples);
		void SetSamplerDescription(const GpuImageSamplerDesc& description);
		void SetMemoryType(GpuSharedMemoryType memoryType);

		/// @brief Resolución de la imagen, en píxeles.
		/// Para imágenes 2D, Z se ignora. 
		/// Para imágenes 1D, Y y Z se ignoran.
		Vector3ui resolution;

		/// @brief Formato de la imagen.
		Format format;

		/// @brief Tareas para las que se usará la imagen.
		GpuImageUsage usage;

		/// @brief Número de dimensiones de la imagen.
		GpuImageDimension dimension;

		/// @brief Número de capas.
		/// Si se trata de una imagen sencilla, debe ser 1.
		/// Si se trata de un array de imágenes, debe ser >= 2.
		/// 
		/// @pre Debe ser > 0.
		TSize numLayers = 1;

		/// @brief Número de muestreos.
		/// Para imágenes normales, 1.
		TSize msaaSamples = 1;

		/// @brief Estructura con información sobre cómo se accederá a la imagen desde los shaders.
		GpuImageSamplerDesc samplerDesc = {};

		GpuSharedMemoryType memoryType = GpuSharedMemoryType::GPU_ONLY;

	};


	/// @brief El asignador de memoria se encarga de reservar grandes bloques
	/// de memoria en la GPU, que después podrán ser usados para
	/// almacenar texturas, buffers, etc.
	/// 
	/// La memoria de un bloque se divide en subbloques. Al reservar
	/// un recurso, se crea un subbloque que ocupará parte de la
	/// memoria del bloque.
	class OSKAPI_CALL IGpuMemoryAllocator {

	public:

		IGpuMemoryAllocator(IGpu* device);
		virtual ~IGpuMemoryAllocator();

		/// <summary>
		/// El tamaño de memoria que, por defecto, tendrá cada bloque de memoria.
		/// 
		/// @note Un bloque puede tener más tamaño, si se usa para almacenar un
		/// recurso que ocupe más espacio.
		/// </summary>
		static TSize SizeOfMemoryBlockInMb;

		OSK_DEFINE_AS(IGpuMemoryAllocator);

		
		/// @brief Crea una nueva imagen en la GPU.
		/// @param info Información para la creación de la imagen.
		virtual OwnedPtr<GpuImage> CreateImage(const GpuImageCreateInfo& info) = 0;

		/// @brief Crea una imagen cubemap sin inicializar en la GPU.
		/// @param faceSize Resolución de cada cara, en píxeles.
		/// @param format Formato de todas las caras.
		/// @param usage Uso que se le va a dar al cubemap.
		/// @param sharedType Tipo de memoria preferente.
		/// @param samplerDesc Sampler del cubemap.
		/// @return Imagen sin información.
		/// 
		/// @post La imagen devuelta tendrá al menos el uso GpuImageUsage::CUBEMAP.
		OwnedPtr<GpuImage> CreateCubemapImage(
			const Vector2ui& faceSize, 
			Format format, 
			GpuImageUsage usage, 
			GpuSharedMemoryType sharedType, 
			GpuImageSamplerDesc samplerDesc = {});

		/// @brief Crea un buffer de vértices con los vértices dados.
		/// @param data Puntero al array de vértices.
		/// @param vertexSize Tamaño de cada vértice individual, en bytes.
		/// @param numVertices Número de vértices pasados.
		/// @param vertexInfo Información del tipo de vértice.
		/// @param usage Uso que se le dará al buffer, además de como vertex buffer.
		/// @return Buffer en la GPU con los vértices.
		/// 
		/// @note El buffer se colocará preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendrá al menos los usos GpuBufferUsage::VERTEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(
			const void* data, 
			TSize vertexSize, 
			TSize numVertices, 
			const VertexInfo& vertexInfo,
			GpuBufferUsage usage = GpuBufferUsage::VERTEX_BUFFER);

		/// @brief Crea un buffer de vértices con los vértices dados.
		/// @tparam T Tipo del vértice.
		/// @param vertices Lista con los vértices a añadir.
		/// @param vertexInfo Información del tipo del vértice.
		/// @param usage Uso que se le dará al buffer, además de como vertex buffer.
		/// @return Buffer en la GPU con los vértices.
		/// 
		/// @note El buffer se colocará preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendrá al menos los usos GpuBufferUsage::VERTEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		template <typename T> 
		inline OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(
			const DynamicArray<T>& vertices, 
			const VertexInfo& vertexInfo,
			GpuBufferUsage usage = GpuBufferUsage::VERTEX_BUFFER) {
			return this->CreateVertexBuffer(vertices.GetData(), sizeof(T), vertices.GetSize(), vertexInfo);
		}

		/// @brief Crea un buffer de índices con los índices dados.
		/// @param indices Lista de índices.
		/// @param usage Uso que se le dará al buffer, además de como index buffer.
		/// @return Buffer con los índices.
		/// 
		/// @note El buffer se colocará preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendrá al menos los usos GpuBufferUsage::INDEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		OwnedPtr<IGpuIndexBuffer> CreateIndexBuffer(
			const DynamicArray<TIndexSize>& indices,
			GpuBufferUsage usage = GpuBufferUsage::INDEX_BUFFER);

		/// @brief Crea un uniform buffer.
		/// @param size Tamaño del uniform buffer.
		/// @param usage Uso que se le dará al buffer, además de como uniform buffer.
		/// @return Uniform buffer.
		/// 
		/// @note El buffer se colocará en un bloque con GpuSharedMemoryType::GPU_AND_CPU.
		/// @post El buffer siempre tendrá al menos el uso GpuBufferUsage::UNIFORM_BUFFER.
		OwnedPtr<IGpuUniformBuffer> CreateUniformBuffer(
			TSize size,
			GpuBufferUsage usage = GpuBufferUsage::UNIFORM_BUFFER);

		/// @brief Crea un storage buffer.
		/// @param size Tamaño, en bytes.
		/// @param usage Uso que se le dará al buffer, además de como storage buffer.
		/// @return Storage buffer.
		/// 
		/// @note El buffer se colocará en un bloque con GpuSharedMemoryType::GPU_AND_CPU.
		/// @post El buffer siempre tendrá al menos el uso GpuBufferUsage::STORAGE_BUFFER.
		OwnedPtr<IGpuStorageBuffer> CreateStorageBuffer(
			TSize size,
			GpuBufferUsage usage = GpuBufferUsage::STORAGE_BUFFER);

		/// @brief Crea un buffer temporal con el tamaño dado.
		/// 
		/// Este buffer alojará memoria en una región donde la CPU tiene acceso,
		/// de tal manera que se podrán enviar datos a esta región, y después
		/// la GPU podrá copiar esta región de memoria a una región de memoria de video
		/// más rápida.
		/// @param size Tamaño del buffer, en bytes.
		/// @param usage Uso que se le va a dar al buffer, además de como staging buffer.
		/// @return Staging buffer.
		/// 
		/// @note El buffer se colocará en un bloque con GpuSharedMemoryType::GPU_AND_CPU.
		/// @post El buffer siempre tendrá al menos los usos GpuBufferUsage::TRANSFER_SOURCE | GpuBufferUsage::UPLOAD_ONLY.
		OwnedPtr<GpuDataBuffer> CreateStagingBuffer(
			TSize size,
			GpuBufferUsage usage = GpuBufferUsage::UPLOAD_ONLY);

		/// @brief Crea un buffer genérico de memoria en la GPU con las características dadas.
		/// @param size Tamaño del buffer, en bytes.
		/// @param alignment Alineamiento del buffer respecto al inicio del bloque.
		/// @param usage Uso que se le dará al buffer.
		/// @param memoryType Tipo de memoria que se alojará.
		/// @return Buffer en la GPU.
		OwnedPtr<GpuDataBuffer> CreateBuffer(
			TSize size, 
			TSize alignment, 
			GpuBufferUsage usage, 
			GpuSharedMemoryType memoryType);


		/// @brief Crea una estructura de aceleración espacial para el trazado de rayos.
		/// 
		/// La estructura será de bajo nivel y contendrá la geometría definida
		/// por los vértices e índices dados.
		/// @param vertexBuffer Vértices del modelo 3D a partir del que se va a crear la estructura.
		/// @param indexBuffer Índices del modelo 3D a partir del que se va a crear la estructura.
		/// @return Estructura de bajo nivel.
		OwnedPtr<IBottomLevelAccelerationStructure> CreateBottomAccelerationStructure(
			const IGpuVertexBuffer& vertexBuffer, 
			const IGpuIndexBuffer& indexBuffer);

		/// @brief Crea una estructura de aceleración espacial para el trazado de rayos.
		/// 
		/// La estructura agrupará varias estructuras de bajo nivel.
		/// @param bottomStructures Lista con todas las estructuras de bajo nivel a partir
		/// de los que se va a crear la estructura de alto nivel.
		/// @return Estructura de alto nivel.
		OwnedPtr<ITopLevelAccelerationStructure> CreateTopAccelerationStructure(
			DynamicArray<IBottomLevelAccelerationStructure*> bottomStructures);


		/// <summary>
		/// Quita uno de los bloques, que ha sido eliminado.
		/// </summary>
		void RemoveMemoryBlock(IGpuMemoryBlock* block);

		/// @brief Libera toda la memoria usada para subir recursos.
		void FreeStagingMemory();

	protected:

		TSize GetAlignment(TSize originalAlignment, GpuBufferUsage usage) const;

		TSize minVertexBufferAlignment = 0;
		TSize minIndexBufferAlignment = 0;
		TSize minUniformBufferAlignment = 0;
		TSize minStorageBufferAlignment = 0;

		//
		
		
		virtual OwnedPtr<IGpuVertexBuffer> _CreateVertexBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock, 
			TSize bufferSize,
			TSize alignment,
			TSize numVertices,
			const VertexInfo& vertexInfo) = 0;


		virtual OwnedPtr<IGpuIndexBuffer> _CreateIndexBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock,
			TSize bufferSize,
			TSize alignment, 
			TSize numIndices) = 0;


		virtual OwnedPtr<IGpuUniformBuffer> _CreateUniformBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock,
			TSize bufferSize,
			TSize alignment) = 0;


		virtual OwnedPtr<IGpuStorageBuffer> _CreateStorageBuffer(
			OwnedPtr<IGpuMemorySubblock> subblock,
			TSize bufferSize,
			TSize alignment) = 0;

		virtual OwnedPtr<IBottomLevelAccelerationStructure> _CreateBottomAccelerationStructure() = 0;


		virtual OwnedPtr<ITopLevelAccelerationStructure> _CreateTopAccelerationStructure() = 0;


		virtual OwnedPtr<IGpuMemoryBlock> CreateNewBufferBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;
		virtual OwnedPtr<IGpuMemoryBlock> CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) = 0;

		/// @brief Devuelve un bloque con las características dadas.
		/// 
		/// @param size Tamaño míniimo del bloque, en bytes.
		/// @param usage Uso que se le dará a la memoria.
		/// @param sharedType Tipo de memoria.
		/// @return Bloque con, al menos, el tamaño dado.
		/// 
		/// @remark Es posible que el tamaño del bloque sea mayor al indicado en size.
		/// @remark Si no hay ningún bloque con tamaño suficiente, se crea uno nuevo.
		IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// @brief Devuelve un subbloque con las características dadas.
		/// @param size Tamaño del subbloque, en bytes.
		/// @param usage Uso que se le dará a la memoria.
		/// @param sharedType Tipo de memoria.
		/// @return Bloque con el tamaño dado.
		IGpuMemorySubblock* GetNextBufferMemorySubblock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// @brief Información de los bloques.
		DynamicArray<GpuBufferMemoryBlockInfo> bufferMemoryBlocksInfo;
		DynamicArray<DynamicArray<UniquePtr<IGpuMemoryBlock>>> bufferMemoryBlocks;

		DynamicArray<UniquePtr<IGpuMemoryBlock>> imageMemoryBlocks;

		IGpu* device = nullptr;

	};

}

template <> static size_t OSK::Hash<OSK::GRAPHICS::GpuBufferMemoryBlockInfo>(const OSK::GRAPHICS::GpuBufferMemoryBlockInfo& elem) {
	return Hash<TSize>((TSize)elem.usage);
}
