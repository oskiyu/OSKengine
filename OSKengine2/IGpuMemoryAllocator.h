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

#include "VertexBufferView.h"
#include "IndexBufferView.h"

#include "GpuMemoryUsageInfo.h"

namespace OSK::GRAPHICS {

	class IGpuMemoryBlock;
	class IGpuMemorySubblock;
	class IGpu;
	enum class GpuBufferUsage;

	class IBottomLevelAccelerationStructure;
	class ITopLevelAccelerationStructure;
	class GpuBuffer;
	class GpuImage;
	struct Vertex3D;
	class VertexInfo;

	struct OSKAPI_CALL GpuBufferMemoryBlockInfo {
		USize64 size;
		GpuBufferUsage usage;
		GpuSharedMemoryType sharedType;

		bool operator==(const GpuBufferMemoryBlockInfo& other) const;
	};

	struct GpuImageCreateInfo {

		static GpuImageCreateInfo CreateDefault1D(uint32_t resolution, Format format, GpuImageUsage usage);
		static GpuImageCreateInfo CreateDefault2D(const Vector2ui& resolution, Format format, GpuImageUsage usage);
		static GpuImageCreateInfo CreateDefault3D(const Vector3ui& resolution, Format format, GpuImageUsage usage);

		void SetMsaaSamples(USize32 msaaSamples);
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
		USize32 numLayers = 1;

		/// @brief N�mero de muestreos.
		/// Para im�genes normales, 1.
		USize32 msaaSamples = 1;

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

		explicit IGpuMemoryAllocator(IGpu* device);
		virtual ~IGpuMemoryAllocator();

		/// <summary>
		/// El tama�o de memoria que, por defecto, tendr� cada bloque de memoria.
		/// 
		/// @note Un bloque puede tener m�s tama�o, si se usa para almacenar un
		/// recurso que ocupe m�s espacio.
		/// </summary>
		static const USize64 SizeOfMemoryBlockInMb;

		OSK_DEFINE_AS(IGpuMemoryAllocator);


		/// @brief Obtiene la informaci�n sobre el uso de memoria de esta GPU,
		/// incluyendo espacio usado y espacio disponible.
		/// @return Informaci�n sobre el uso de memoria de esta GPU.
		GpuMemoryUsageInfo GetMemoryUsageInfo() const;
		
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
		OwnedPtr<GpuBuffer> CreateVertexBuffer(
			const void* data, 
			USize32 vertexSize, 
			USize32 numVertices, 
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
		inline OwnedPtr<GpuBuffer> CreateVertexBuffer(
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
		OwnedPtr<GpuBuffer> CreateIndexBuffer(
			const DynamicArray<TIndexSize>& indices,
			GpuBufferUsage usage = GpuBufferUsage::INDEX_BUFFER);

		/// @brief Crea un uniform buffer.
		/// @param size Tama�o del uniform buffer.
		/// @param usage Uso que se le dar� al buffer, adem�s de como uniform buffer.
		/// @return Uniform buffer.
		/// 
		/// @note El buffer se colocar� en un bloque con GpuSharedMemoryType::GPU_AND_CPU.
		/// @post El buffer siempre tendr� al menos el uso GpuBufferUsage::UNIFORM_BUFFER.
		OwnedPtr<GpuBuffer> CreateUniformBuffer(
			USize64 size,
			GpuBufferUsage usage = GpuBufferUsage::UNIFORM_BUFFER);

		/// @brief Crea un storage buffer.
		/// @param size Tama�o, en bytes.
		/// @param usage Uso que se le dar� al buffer, adem�s de como storage buffer.
		/// @return Storage buffer.
		/// 
		/// @note El buffer se colocar� en un bloque con GpuSharedMemoryType::GPU_AND_CPU.
		/// @post El buffer siempre tendr� al menos el uso GpuBufferUsage::STORAGE_BUFFER.
		OwnedPtr<GpuBuffer> CreateStorageBuffer(
			USize64 size,
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
		OwnedPtr<GpuBuffer> CreateStagingBuffer(
			USize64 size,
			GpuBufferUsage usage = GpuBufferUsage::UPLOAD_ONLY);

		/// @brief Crea un buffer gen�rico de memoria en la GPU con las caracter�sticas dadas.
		/// @param size Tama�o del buffer, en bytes.
		/// @param alignment Alineamiento del buffer respecto al inicio del bloque.
		/// @param usage Uso que se le dar� al buffer.
		/// @param memoryType Tipo de memoria que se alojar�.
		/// @return Buffer en la GPU.
		OwnedPtr<GpuBuffer> CreateBuffer(
			USize64 size,
			USize64 alignment,
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
			const GpuBuffer& vertexBuffer,
			const GpuBuffer& indexBuffer);

		/// @brief Crea una estructura de aceleraci�n espacial para el trazado de rayos.
		/// 
		/// La estructura ser� de bajo nivel y contendr� la geometr�a definida
		/// por los v�rtices e �ndices dados.
		/// @param vertexBuffer V�rtices del modelo 3D a partir del que se va a crear la estructura.
		/// @param vertexView View de los v�rtices.
		/// @param indexBuffer �ndices del modelo 3D a partir del que se va a crear la estructura.
		/// @param indexView View de los �ndices.
		/// @return Estructura de bajo nivel.
		OwnedPtr<IBottomLevelAccelerationStructure> CreateBottomAccelerationStructure(
			const GpuBuffer& vertexBuffer,
			const VertexBufferView& vertexView,
			const GpuBuffer& indexBuffer,
			const IndexBufferView& indexView);

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

		USize64 GetAlignment(USize64 originalAlignment, GpuBufferUsage usage) const;

		USize64 minVertexBufferAlignment = 0;
		USize64 minIndexBufferAlignment = 0;
		USize64 minUniformBufferAlignment = 0;
		USize64 minStorageBufferAlignment = 0;

		//
		
		
		virtual OwnedPtr<IBottomLevelAccelerationStructure> _CreateBottomAccelerationStructure() = 0;


		virtual OwnedPtr<ITopLevelAccelerationStructure> _CreateTopAccelerationStructure() = 0;


		virtual OwnedPtr<IGpuMemoryBlock> CreateNewBufferBlock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) = 0;
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
		IGpuMemoryBlock* GetNextBufferMemoryBlock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// @brief Devuelve un subbloque con las caracter�sticas dadas.
		/// @param size Tama�o del subbloque, en bytes.
		/// @param usage Uso que se le dar� a la memoria.
		/// @param sharedType Tipo de memoria.
		/// @return Bloque con el tama�o dado.
		IGpuMemorySubblock* GetNextBufferMemorySubblock(USize64 size, GpuBufferUsage usage, GpuSharedMemoryType sharedType);

		/// @brief Informaci�n de los bloques.
		DynamicArray<GpuBufferMemoryBlockInfo> bufferMemoryBlocksInfo;
		DynamicArray<DynamicArray<UniquePtr<IGpuMemoryBlock>>> bufferMemoryBlocks;

		DynamicArray<UniquePtr<IGpuMemoryBlock>> imageMemoryBlocks;

		IGpu* device = nullptr;

	};

}

template <> static size_t OSK::Hash<OSK::GRAPHICS::GpuBufferMemoryBlockInfo>(const OSK::GRAPHICS::GpuBufferMemoryBlockInfo& elem) {
	return Hash<size_t>((size_t)elem.usage);
}
