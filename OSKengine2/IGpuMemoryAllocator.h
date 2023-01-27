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

		/// <summary>
		/// Crea una imagen cubemap en la GPU.
		/// </summary>
		OwnedPtr<GpuImage> CreateCubemapImage(const Vector2ui& faceSize, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, GpuImageSamplerDesc samplerDesc = {});

		/// <summary>
		/// Crea un buffer de vértices con los vértices dados.
		/// </summary>
		virtual OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const void* data, TSize vertexSize, TSize numVertices, const VertexInfo& vertexInfo) = 0;

		/// <summary>
		/// Crea un buffer de vértices con los vértices dados.
		/// </summary>
		template <typename T> OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const DynamicArray<T>& vertices, const VertexInfo& vertexInfo) {
			return this->CreateVertexBuffer(vertices.GetData(), sizeof(T), vertices.GetSize(), vertexInfo);
		}

		/// <summary>
		/// Crea un buffer de índices con los índices dados.
		/// </summary>
		virtual OwnedPtr<IGpuIndexBuffer> CreateIndexBuffer(const DynamicArray<TIndexSize>& indices) = 0;

		/// <summary>
		/// Crea un uniform buffer, con el tamaño dado en bytes.
		/// </summary>
		virtual OwnedPtr<IGpuUniformBuffer> CreateUniformBuffer(TSize size) = 0;

		/// <summary>
		/// Crea un storage buffer, con el tamaño dado en bytes.
		/// </summary>
		virtual OwnedPtr<IGpuStorageBuffer> CreateStorageBuffer(TSize size) = 0;

		/// <summary>
		/// Crea un buffer temporal con el tamaño dado.
		/// 
		/// Este buffer alojará memoria en una región donde la CPU tiene acceso,
		/// de tal manera que se podrán enviar datos a esta región, y después
		/// la GPU podrá copiar esta región de memoria a una región de memoria de video
		/// más rápida.
		/// </summary>
		virtual OwnedPtr<GpuDataBuffer> CreateStagingBuffer(TSize size) = 0;

		/// <summary>
		/// Crea un buffer genérico con las características dadas.
		/// </summary>
		/// <param name="size">Tamaño del buffer, en bytes.</param>
		/// <param name="usage">Uso que se le dará al buffer.</param>
		/// <param name="sharedType">Configura si se accederá únicamente desde la GPU, o si también
		/// se accederá desde la CPU.</param>
		virtual OwnedPtr<GpuDataBuffer> CreateBuffer(TSize size, TSize alignment, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;

		/// <summary>
		/// Crea una estructura de aceleración espacial para el trazado de rayos.
		/// 
		/// La estructura será de bajo nivel y contendrá la geometría definida
		/// por los vértices e índices dados.
		/// </summary>
		virtual OwnedPtr<IBottomLevelAccelerationStructure> CreateBottomAccelerationStructure(const IGpuVertexBuffer& vertexBuffer, const IGpuIndexBuffer& indexBuffer) = 0;

		/// <summary>
		/// Crea una estructura de aceleración espacial para el trazado de rayos.
		/// 
		/// La estructura agrupará varias estructuras de bajo nivel.
		/// </summary>
		virtual OwnedPtr<ITopLevelAccelerationStructure> CreateTopAccelerationStructure(DynamicArray<const IBottomLevelAccelerationStructure*> bottomStructures) = 0;

		/// <summary>
		/// Quita uno de los bloques, que ha sido eliminado.
		/// </summary>
		void RemoveMemoryBlock(IGpuMemoryBlock* block);

	protected:

		virtual OwnedPtr<IGpuMemoryBlock> CreateNewBufferBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;
		virtual OwnedPtr<IGpuMemoryBlock> CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) = 0;

		/// <summary>
		/// Devuelve un bloque con las características dadas.
		/// 
		/// Si no hay ningún bloque con tamaño suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tamaño libre necesitado.</param>
		IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// <summary>
		/// Devuelve un subbloque con las características dadas.
		/// 
		/// Si no hay ningún subbloque con tamaño suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tamaño libre necesitado.</param>
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
