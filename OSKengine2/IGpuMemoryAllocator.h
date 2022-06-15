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
	/// de memoria en la GPU, que despu�s podr�n ser usados para
	/// almacenar texturas, buffers, etc.
	/// 
	/// La memoria de un bloque se divide en subbloques. Al reservar
	/// un recurso, se crea un subbloque que ocupar� parte de la
	/// memoria del bloque.
	/// </summary>
	class OSKAPI_CALL IGpuMemoryAllocator {

	public:

		IGpuMemoryAllocator(IGpu* device);
		virtual ~IGpuMemoryAllocator();

		/// <summary>
		/// El tama�o de memoria que, por defecto, tendr� cada bloque de memoria.
		/// 
		/// @note Un bloque puede tener m�s tama�o, si se usa para almacenar un
		/// recurso que ocupe m�s espacio.
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
		/// Tama�o de la imagen. 
		/// @note Para im�genes 2D, Z se ignora. 
		/// @note Para im�genes 1D, Y y Z se ignoran.
		/// </param>
		/// <param name="dimension">Dimensi�n de la imagen.</param>
		/// <param name="numLayers">
		/// N�mero de capas.
		/// @note  1 es una imagen sencilla.
		/// @note 2 o m�s forman un array de im�genes.
		/// @pre Debe ser mayor que 0.</param>
		/// <param name="format">Formato.</param>
		/// <param name="usage">Uso que se le va a dar.</param>
		/// <param name="sharedType">GPU o GPU-CPU.</param>
		/// <param name="msaaSamples">N�mero de muestreos. 
		/// @note Para im�genes normales, 1.</param>
		/// <param name="samplerDesc">Descipci�n del sampler.</param>
		virtual OwnedPtr<GpuImage> CreateImage(const Vector3ui& size, GpuImageDimension dimension, TSize numLayers, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, TSize msaaSamples, GpuImageSamplerDesc samplerDesc = {}) = 0;

		/// <summary>
		/// Crea una imagen cubemap en la GPU.
		/// </summary>
		OwnedPtr<GpuImage> CreateCubemapImage(const Vector2ui& faceSize, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, GpuImageSamplerDesc samplerDesc = {});

		/// <summary>
		/// Crea un buffer de v�rtices con los v�rtices dados.
		/// </summary>
		virtual OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const void* data, TSize vertexSize, TSize numVertices, const VertexInfo& vertexInfo) = 0;

		/// <summary>
		/// Crea un buffer de v�rtices con los v�rtices dados.
		/// </summary>
		template <typename T> OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(const DynamicArray<T>& vertices, const VertexInfo& vertexInfo) {
			return this->CreateVertexBuffer(vertices.GetData(), sizeof(T), vertices.GetSize(), vertexInfo);
		}

		/// <summary>
		/// Crea un buffer de �ndices con los �ndices dados.
		/// </summary>
		virtual OwnedPtr<IGpuIndexBuffer> CreateIndexBuffer(const DynamicArray<TIndexSize>& indices) = 0;

		/// <summary>
		/// Crea un uniform buffer, con el tama�o dado en bytes.
		/// </summary>
		virtual OwnedPtr<IGpuUniformBuffer> CreateUniformBuffer(TSize size) = 0;

		/// <summary>
		/// Crea un buffer temporal con el tama�o dado.
		/// 
		/// Este buffer alojar� memoria en una regi�n donde la CPU tiene acceso,
		/// de tal manera que se podr�n enviar datos a esta regi�n, y despu�s
		/// la GPU podr� copiar esta regi�n de memoria a una regi�n de memoria de video
		/// m�s r�pida.
		/// </summary>
		virtual OwnedPtr<GpuDataBuffer> CreateStagingBuffer(TSize size) = 0;

		/// <summary>
		/// Crea un buffer gen�rico con las caracter�sticas dadas.
		/// </summary>
		/// <param name="size">Tama�o del buffer, en bytes.</param>
		/// <param name="usage">Uso que se le dar� al buffer.</param>
		/// <param name="sharedType">Configura si se acceder� �nicamente desde la GPU, o si tambi�n
		/// se acceder� desde la CPU.</param>
		virtual OwnedPtr<GpuDataBuffer> CreateBuffer(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;

		/// <summary>
		/// Crea una estructura de aceleraci�n espacial para el trazado de rayos.
		/// 
		/// La estructura ser� de bajo nivel y contendr� la geometr�a definida
		/// por los v�rtices e �ndices dados.
		/// </summary>
		virtual OwnedPtr<IBottomLevelAccelerationStructure> CreateBottomAccelerationStructure(const IGpuVertexBuffer& vertexBuffer, const IGpuIndexBuffer& indexBuffer) = 0;

		/// <summary>
		/// Crea una estructura de aceleraci�n espacial para el trazado de rayos.
		/// 
		/// La estructura agrupar� varias estructuras de bajo nivel.
		/// </summary>
		virtual OwnedPtr<ITopLevelAccelerationStructure> CreateTopAccelerationStructure(DynamicArray<const IBottomLevelAccelerationStructure*> bottomStructures) = 0;

		/// <summary>
		/// Quita uno de los bloques, que ha sido eliminado.
		/// </summary>
		void RemoveImageBlock(IGpuMemoryBlock* block);

	protected:

		/// <summary>
		/// Devuelve un bloque con las caracter�sticas dadas.
		/// 
		/// Si no hay ning�n bloque con tama�o suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tama�o libre.</param>
		virtual IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;

		/// <summary>
		/// Devuelve un subbloque con las caracter�sticas dadas.
		/// 
		/// Si no hay ning�n subbloque con tama�o suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tama�o libre.</param>
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
