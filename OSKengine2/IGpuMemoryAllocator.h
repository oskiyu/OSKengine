#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "LinkedList.hpp"
#include "HashMap.hpp"
#include "DynamicArray.hpp"
#include "Vertex.h"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "GpuImageSamplerDesc.h"

namespace OSK::GRAPHICS {

	enum class Format;
	class IGpuMemoryBlock;
	class IGpuMemorySubblock;
	class IGpu;
	enum class GpuSharedMemoryType;
	enum class GpuBufferUsage;
	enum class GpuImageUsage;
	enum class GpuImageDimension;

	class IBottomLevelAccelerationStructure;
	class ITopLevelAccelerationStructure;
	class GpuDataBuffer;
	class GpuImage;
	class IGpuVertexBuffer;
	class IGpuIndexBuffer;
	class IGpuUniformBuffer;
	struct Vertex3D;
	class VertexInfo;

	struct OSKAPI_CALL GpuBufferMemoryBlockInfo {
		unsigned int size;
		GpuBufferUsage usage;
		GpuSharedMemoryType sharedType;

		bool operator==(const GpuBufferMemoryBlockInfo& other) const;
	};

	/// <summary>
	/// El asignador de memoria se encarga de reservar grandes bloques
	/// de memoria en la GPU, que después podrán ser usados para
	/// almacenar texturas, buffers, etc.
	/// 
	/// La memoria de un bloque se divide en subbloques. Al reservar
	/// un recurso, se crea un subbloque que ocupará parte de la
	/// memoria del bloque.
	/// </summary>
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

		/// <summary>
		/// Libera toda la memoria.
		/// Libera todos los bloques.
		/// </summary>
		void Free();

		template <typename T> T* As() const requires std::is_base_of_v<IGpuMemoryAllocator, T> {
			return (T*)this;
		}

		/// <summary>
		/// Crea una nueva imagen en la GPU.
		/// </summary>
		/// <param name="size">
		/// Tamaño de la imagen. 
		/// @note Para imágenes 2D, Z se ignora. 
		/// @note Para imágenes 1D, Y y Z se ignoran.
		/// </param>
		/// <param name="dimension">Dimensión de la imagen.</param>
		/// <param name="numLayers">
		/// Número de capas.
		/// @note  1 es una imagen sencilla.
		/// @note 2 o más forman un array de imágenes.
		/// @pre Debe ser mayor que 0.</param>
		/// <param name="format">Formato.</param>
		/// <param name="usage">Uso que se le va a dar.</param>
		/// <param name="sharedType">GPU o GPU-CPU.</param>
		/// <param name="msaaSamples">Número de muestreos. 
		/// @note Para imágenes normales, 1.</param>
		/// <param name="samplerDesc">Descipción del sampler.</param>
		virtual OwnedPtr<GpuImage> CreateImage(const Vector3ui& size, GpuImageDimension dimension, TSize numLayers, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, TSize msaaSamples, GpuImageSamplerDesc samplerDesc = {}) = 0;

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
		virtual OwnedPtr<GpuDataBuffer> CreateBuffer(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;

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
		void RemoveImageBlock(IGpuMemoryBlock* block);

	protected:

		/// <summary>
		/// Devuelve un bloque con las características dadas.
		/// 
		/// Si no hay ningún bloque con tamaño suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tamaño libre.</param>
		virtual IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;

		/// <summary>
		/// Devuelve un subbloque con las características dadas.
		/// 
		/// Si no hay ningún subbloque con tamaño suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tamaño libre.</param>
		IGpuMemorySubblock* GetNextBufferMemorySubblock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// <summary>
		/// Bloques ya reservados.
		/// </summary>
		HashMap<GpuBufferMemoryBlockInfo, LinkedList<OwnedPtr<IGpuMemoryBlock>>> bufferMemoryBlocks;
		LinkedList<OwnedPtr<IGpuMemoryBlock>> imageMemoryBlocks;

		IGpu* device = nullptr;

	};

}

template <> static size_t OSK::Hash<OSK::GRAPHICS::GpuBufferMemoryBlockInfo>(const OSK::GRAPHICS::GpuBufferMemoryBlockInfo& elem) {
	return Hash<TSize>((TSize)elem.usage);
}
