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

		/// @brief Resoluci�n de la imagen, en p�xeles.
		/// Para im�genes 2D, Z se ignora. 
		/// Para im�genes 1D, Y y Z se ignoran.
		Vector3ui resolution;

		/// @brief Formato de la imagen.
		Format format;

		/// @brief Tareas para las que se usar� la imagen.
		GpuImageUsage usage;

		/// @brief N�mero de dimensiones de la imagen.
		GpuImageDimension dimension;

		/// @brief N�mero de capas.
		/// Si se trata de una imagen sencilla, debe ser 1.
		/// Si se trata de un array de im�genes, debe ser >= 2.
		/// 
		/// @pre Debe ser > 0.
		TSize numLayers = 1;

		/// @brief N�mero de muestreos.
		/// Para im�genes normales, 1.
		TSize msaaSamples = 1;

		/// @brief Estructura con informaci�n sobre c�mo se acceder� a la imagen desde los shaders.
		GpuImageSamplerDesc samplerDesc = {};

		GpuSharedMemoryType memoryType = GpuSharedMemoryType::GPU_ONLY;

	};


	/// @brief El asignador de memoria se encarga de reservar grandes bloques
	/// de memoria en la GPU, que despu�s podr�n ser usados para
	/// almacenar texturas, buffers, etc.
	/// 
	/// La memoria de un bloque se divide en subbloques. Al reservar
	/// un recurso, se crea un subbloque que ocupar� parte de la
	/// memoria del bloque.
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

		OSK_DEFINE_AS(IGpuMemoryAllocator);

		
		/// @brief Crea una nueva imagen en la GPU.
		/// @param info Informaci�n para la creaci�n de la imagen.
		virtual OwnedPtr<GpuImage> CreateImage(const GpuImageCreateInfo& info) = 0;

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
		/// Crea un storage buffer, con el tama�o dado en bytes.
		/// </summary>
		virtual OwnedPtr<IGpuStorageBuffer> CreateStorageBuffer(TSize size) = 0;

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
		virtual OwnedPtr<GpuDataBuffer> CreateBuffer(TSize size, TSize alignment, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;

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
		void RemoveMemoryBlock(IGpuMemoryBlock* block);

	protected:

		virtual OwnedPtr<IGpuMemoryBlock> CreateNewBufferBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;
		virtual OwnedPtr<IGpuMemoryBlock> CreateNewImageBlock(GpuImage* image, GpuImageUsage usage, GpuSharedMemoryType sharedType) = 0;

		/// <summary>
		/// Devuelve un bloque con las caracter�sticas dadas.
		/// 
		/// Si no hay ning�n bloque con tama�o suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tama�o libre necesitado.</param>
		IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// <summary>
		/// Devuelve un subbloque con las caracter�sticas dadas.
		/// 
		/// Si no hay ning�n subbloque con tama�o suficiente, se crea uno nuevo.
		/// </summary>
		/// <param name="size">Tama�o libre necesitado.</param>
		IGpuMemorySubblock* GetNextBufferMemorySubblock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// @brief Informaci�n de los bloques.
		DynamicArray<GpuBufferMemoryBlockInfo> bufferMemoryBlocksInfo;
		DynamicArray<DynamicArray<UniquePtr<IGpuMemoryBlock>>> bufferMemoryBlocks;

		DynamicArray<UniquePtr<IGpuMemoryBlock>> imageMemoryBlocks;

		IGpu* device = nullptr;

	};

}

template <> static size_t OSK::Hash<OSK::GRAPHICS::GpuBufferMemoryBlockInfo>(const OSK::GRAPHICS::GpuBufferMemoryBlockInfo& elem) {
	return Hash<TSize>((TSize)elem.usage);
}
