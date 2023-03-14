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

		/// @brief Crea una imagen cubemap sin inicializar en la GPU.
		/// @param faceSize Resoluci�n de cada cara, en p�xeles.
		/// @param format Formato de todas las caras.
		/// @param usage Uso que se le va a dar al cubemap.
		/// @param sharedType Tipo de memoria preferente.
		/// @param samplerDesc Sampler del cubemap.
		/// @return Imagen sin informaci�n.
		/// 
		/// @post La imagen devuelta tendr� al menos el uso GpuImageUsage::CUBEMAP.
		OwnedPtr<GpuImage> CreateCubemapImage(
			const Vector2ui& faceSize, 
			Format format, 
			GpuImageUsage usage, 
			GpuSharedMemoryType sharedType, 
			GpuImageSamplerDesc samplerDesc = {});

		/// @brief Crea un buffer de v�rtices con los v�rtices dados.
		/// @param data Puntero al array de v�rtices.
		/// @param vertexSize Tama�o de cada v�rtice individual, en bytes.
		/// @param numVertices N�mero de v�rtices pasados.
		/// @param vertexInfo Informaci�n del tipo de v�rtice.
		/// @param usage Uso que se le dar� al buffer, adem�s de como vertex buffer.
		/// @return Buffer en la GPU con los v�rtices.
		/// 
		/// @note El buffer se colocar� preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendr� al menos los usos GpuBufferUsage::VERTEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(
			const void* data, 
			TSize vertexSize, 
			TSize numVertices, 
			const VertexInfo& vertexInfo,
			GpuBufferUsage usage = GpuBufferUsage::VERTEX_BUFFER);

		/// @brief Crea un buffer de v�rtices con los v�rtices dados.
		/// @tparam T Tipo del v�rtice.
		/// @param vertices Lista con los v�rtices a a�adir.
		/// @param vertexInfo Informaci�n del tipo del v�rtice.
		/// @param usage Uso que se le dar� al buffer, adem�s de como vertex buffer.
		/// @return Buffer en la GPU con los v�rtices.
		/// 
		/// @note El buffer se colocar� preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendr� al menos los usos GpuBufferUsage::VERTEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		template <typename T> 
		inline OwnedPtr<IGpuVertexBuffer> CreateVertexBuffer(
			const DynamicArray<T>& vertices, 
			const VertexInfo& vertexInfo,
			GpuBufferUsage usage = GpuBufferUsage::VERTEX_BUFFER) {
			return this->CreateVertexBuffer(vertices.GetData(), sizeof(T), vertices.GetSize(), vertexInfo);
		}

		/// @brief Crea un buffer de �ndices con los �ndices dados.
		/// @param indices Lista de �ndices.
		/// @param usage Uso que se le dar� al buffer, adem�s de como index buffer.
		/// @return Buffer con los �ndices.
		/// 
		/// @note El buffer se colocar� preferentemente en un bloque con GpuSharedMemoryType::GPU_ONLY.
		/// @post El buffer siempre tendr� al menos los usos GpuBufferUsage::INDEX_BUFFER y GpuBufferUsage::TRANSFER_DESTINATION.
		OwnedPtr<IGpuIndexBuffer> CreateIndexBuffer(
			const DynamicArray<TIndexSize>& indices,
			GpuBufferUsage usage = GpuBufferUsage::INDEX_BUFFER);

		/// @brief Crea un uniform buffer.
		/// @param size Tama�o del uniform buffer.
		/// @param usage Uso que se le dar� al buffer, adem�s de como uniform buffer.
		/// @return Uniform buffer.
		/// 
		/// @note El buffer se colocar� en un bloque con GpuSharedMemoryType::GPU_AND_CPU.
		/// @post El buffer siempre tendr� al menos el uso GpuBufferUsage::UNIFORM_BUFFER.
		OwnedPtr<IGpuUniformBuffer> CreateUniformBuffer(
			TSize size,
			GpuBufferUsage usage = GpuBufferUsage::UNIFORM_BUFFER);

		/// @brief Crea un storage buffer.
		/// @param size Tama�o, en bytes.
		/// @param usage Uso que se le dar� al buffer, adem�s de como storage buffer.
		/// @return Storage buffer.
		/// 
		/// @note El buffer se colocar� en un bloque con GpuSharedMemoryType::GPU_AND_CPU.
		/// @post El buffer siempre tendr� al menos el uso GpuBufferUsage::STORAGE_BUFFER.
		OwnedPtr<IGpuStorageBuffer> CreateStorageBuffer(
			TSize size,
			GpuBufferUsage usage = GpuBufferUsage::STORAGE_BUFFER);

		/// @brief Crea un buffer temporal con el tama�o dado.
		/// 
		/// Este buffer alojar� memoria en una regi�n donde la CPU tiene acceso,
		/// de tal manera que se podr�n enviar datos a esta regi�n, y despu�s
		/// la GPU podr� copiar esta regi�n de memoria a una regi�n de memoria de video
		/// m�s r�pida.
		/// @param size Tama�o del buffer, en bytes.
		/// @param usage Uso que se le va a dar al buffer, adem�s de como staging buffer.
		/// @return Staging buffer.
		/// 
		/// @note El buffer se colocar� en un bloque con GpuSharedMemoryType::GPU_AND_CPU.
		/// @post El buffer siempre tendr� al menos los usos GpuBufferUsage::TRANSFER_SOURCE | GpuBufferUsage::UPLOAD_ONLY.
		OwnedPtr<GpuDataBuffer> CreateStagingBuffer(
			TSize size,
			GpuBufferUsage usage = GpuBufferUsage::UPLOAD_ONLY);

		/// @brief Crea un buffer gen�rico de memoria en la GPU con las caracter�sticas dadas.
		/// @param size Tama�o del buffer, en bytes.
		/// @param alignment Alineamiento del buffer respecto al inicio del bloque.
		/// @param usage Uso que se le dar� al buffer.
		/// @param memoryType Tipo de memoria que se alojar�.
		/// @return Buffer en la GPU.
		OwnedPtr<GpuDataBuffer> CreateBuffer(
			TSize size, 
			TSize alignment, 
			GpuBufferUsage usage, 
			GpuSharedMemoryType memoryType);


		/// @brief Crea una estructura de aceleraci�n espacial para el trazado de rayos.
		/// 
		/// La estructura ser� de bajo nivel y contendr� la geometr�a definida
		/// por los v�rtices e �ndices dados.
		/// @param vertexBuffer V�rtices del modelo 3D a partir del que se va a crear la estructura.
		/// @param indexBuffer �ndices del modelo 3D a partir del que se va a crear la estructura.
		/// @return Estructura de bajo nivel.
		OwnedPtr<IBottomLevelAccelerationStructure> CreateBottomAccelerationStructure(
			const IGpuVertexBuffer& vertexBuffer, 
			const IGpuIndexBuffer& indexBuffer);

		/// @brief Crea una estructura de aceleraci�n espacial para el trazado de rayos.
		/// 
		/// La estructura agrupar� varias estructuras de bajo nivel.
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

		/// @brief Devuelve un bloque con las caracter�sticas dadas.
		/// 
		/// @param size Tama�o m�niimo del bloque, en bytes.
		/// @param usage Uso que se le dar� a la memoria.
		/// @param sharedType Tipo de memoria.
		/// @return Bloque con, al menos, el tama�o dado.
		/// 
		/// @remark Es posible que el tama�o del bloque sea mayor al indicado en size.
		/// @remark Si no hay ning�n bloque con tama�o suficiente, se crea uno nuevo.
		IGpuMemoryBlock* GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// @brief Devuelve un subbloque con las caracter�sticas dadas.
		/// @param size Tama�o del subbloque, en bytes.
		/// @param usage Uso que se le dar� a la memoria.
		/// @param sharedType Tipo de memoria.
		/// @return Bloque con el tama�o dado.
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
