#pragma once

#include "OSKmacros.h"
#include "Vector4.hpp"
#include "DynamicArray.hpp"
#include "UniquePtr.hpp"
#include "IGpuDataBuffer.h"
#include "Color.hpp"
#include "RenderpassType.h"
#include "Vector3.hpp"

#include "IGpuObject.h"
#include "GpuBarrierInfo.h"

#include <string>
#include <type_traits>

namespace OSK::GRAPHICS {

	class Material;
	class IMaterialSlot;
	class GpuImage;
	enum class GpuImageLayout;
	class RenderTarget;
	class IGpuVertexBuffer;
	class IGpuIndexBuffer;
	struct Viewport;
	class IGraphicsPipeline;
	class IRaytracingPipeline;
	class IComputePipeline;

	struct RenderPassImageInfo {
		GpuImage* targetImage = nullptr;
		TSize arrayLevel = 0;
	};

	/// @brief Parámetros para la copia de una imagen a otra
	/// en la GPU.
	struct CopyImageInfo {

		static CopyImageInfo CreateDefault1D(uint32_t size);
		static CopyImageInfo CreateDefault2D(Vector2ui size);
		static CopyImageInfo CreateDefault3D(Vector3ui size);

		void SetCopySize(uint32_t size);
		void SetCopySize(Vector2ui size);
		void SetCopySize(Vector3ui size);

		void SetSourceOffset(uint32_t offset);
		void SetSourceOffset(Vector2ui offset);
		void SetSourceOffset(Vector3ui offset);

		void SetDestinationOffset(uint32_t offset);
		void SetDestinationOffset(Vector2ui offset);
		void SetDestinationOffset(Vector3ui offset);

		void SetCopyAllLevels();

		/// @brief Localización del área copiada de la imagen
		/// de origen. Esquina superior izquierda.
		Vector3ui sourceOffset = 0;

		/// @brief Localización del área copiada de la imagen
		/// final. Esquina superior izquierda.
		Vector3ui destinationOffset = 0;

		/// @brief Área copiada.
		Vector3ui copySize = 0;


		/// @brief Capa de origen a partir de la cual se copiará.
		TIndex sourceArrayLevel = 0;

		/// @brief Capa de destino a partir de la cual se escribirá.
		TIndex destinationArrayLevel = 0;

		/// @brief Número de capas que se van a copiar.
		/// Para copiar todas las capas, establecer como ALL_ARRAY_LEVELS.
		TSize numArrayLevels = 1;


		/// @brief Mip level de la imagen de origen desde el cual se copiará.
		TIndex sourceMipLevel = 0;

		/// @brief Mip level de la imagen de destino al que se copiará.
		TIndex destinationMipLevel = 0;

		const static TIndex ALL_ARRAY_LEVELS = 0;

	};

	/// @brief Una lista de comandos contiene una serie de comandos que serán
	/// enviados a la GPU para su ejecución.
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecución.
	/// 
	/// @note Los comandos serán ejecutados en orden una vez se envíe la lista
	/// a la GPU.
	class OSKAPI_CALL ICommandList : public IGpuObject {

	public:

		virtual ~ICommandList();

		OSK_DEFINE_AS(ICommandList);

		/// @brief Vacía la lista de comandos.
		/// 
		/// @pre Debe estar cerrado.
		/// @note Debe llamarse antes de comenzar a grabar comandos.
		virtual void Reset() = 0;

		/// @brief Comienza a grabar comandos en la lista.
		/// 
		/// @pre Debe estar cerrado.
		virtual void Start() = 0;

		/// @brief Cierra la lista de comandos, y la deja lista para ser enviada
		/// a la GPU.
		/// 
		/// @pre Debe estar abierto.
		virtual void Close() = 0;


		/// @brief Establece un barrier que sincroniza la ejecución de comandos.
		/// También cambia el layout de la imagen.
		/// 
		/// @see GpuImageBarrierInfo.
		/// 
		/// @param image Imagen a la que se le cambiará el layout.
		/// @param previousLayout Layout anterior.
		/// @param nextLayout Nuevo layout.
		/// @param previous Stage previo.
		/// @param next Stage siguiente.
		/// @param prevImageInfo Información sobre que subrecursos de la imagen serán afectados.
		///
		/// @note Se debe cambiar el layout de la imagen antes de ejecutar un comando sobre ella,
		/// si su layout actual no coincide con el necesario.
		/// 
		/// @pre La lista de comandos debe estar abierta.
		/// @pre previousLayout debe ser el layout de la imagen antes de este comando.
		/// 
		/// @post Los subrecursos especificados por imageInfo estarán en el nuevo layout.
		virtual void SetGpuImageBarrier(
			GpuImage* image, 
			GpuImageLayout previousLayout, 
			GpuImageLayout nextLayout, 
			GpuBarrierInfo previous, 
			GpuBarrierInfo next, 
			const GpuImageBarrierInfo& prevImageInfo = {}) = 0;

		/// @brief Establece un barrier que sincroniza la ejecución de comandos.
		/// Cambia el layout de la imagen.
		/// 
		/// @param image Imagen a la que se le cambiará el layout.
		/// @param nextLayout Nuevo layout.
		/// @param previous Stage previo.
		/// @param next Stage siguiente.
		/// @param prevImageInfo Información sobre que subrecursos de la imagen serán afectados.
		/// 
		/// @note Se debe cambiar el layout de la imagen antes de ejecutar un comando sobre ella,
		/// si su layout actual no coincide con el necesario.
		/// 
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post Los subrecursos especificados por imageInfo estarán en el nuevo layout.
		void SetGpuImageBarrier(
			GpuImage* image, 
			GpuImageLayout nextLayout, 
			GpuBarrierInfo previous, 
			GpuBarrierInfo next, 
			const GpuImageBarrierInfo& prevImageInfo = {});


		/// @brief Comienza el renderizado a un render target.
		/// @param renderTarget Render target sobre el que se renderizará.
		/// @param color Color con el que limpiará la imagen.
		/// 
		/// @note Por defecto, se limpiará la imagen con color negro.
		/// @note Se encarga de establecer el layout de las imágenes de color y de profundidad.
		/// 
		/// @pre No debe haber ningún renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post Las imágenes de color se encontrarán en GpuImageLayout::COLOR_ATTACHMENT.
		/// @post La imagen de prfundidad se encontrarán en GpuImageLayout::DEPTH_STENCIL_TARGET.
		void BeginGraphicsRenderpass(
			RenderTarget* renderTarget, 
			const Color& color = Color::BLACK());

		/// @brief Comienza el renderizado a las imágenes dadas.
		/// @param colorImages Imágenes de color sobre las que se renderizará.
		/// @param depthImage Imagen de profundidad.
		/// @param color Color con el que limpiará la imagen.
		/// 
		/// @note Por defecto, se limpiará la imagen con color negro.
		/// @note Se encarga de establecer el layout de las imágenes de color y de profundidad.
		/// 
		/// @pre No debe haber ningún renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post Las imágenes de color se encontrarán en GpuImageLayout::COLOR_ATTACHMENT.
		/// @post La imagen de prfundidad se encontrarán en GpuImageLayout::DEPTH_STENCIL_TARGET.
		virtual void BeginGraphicsRenderpass(
			DynamicArray<RenderPassImageInfo> colorImages, 
			RenderPassImageInfo depthImage, 
			const Color& color = Color::BLACK()) = 0;

		/// @brief Finaliza el renderizado a un render target.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// @post Aplica a los color targets un GpuImageBarrier con nuevo layout = SHADER_READ_ONLY para 
		/// fragment shader.
		virtual void EndGraphicsRenderpass() = 0;


		/// @brief Establece el material que se va a usar a la hora de renderizar los próximos comandos.
		///
		/// @note Los slots que se enlacen después de este comando deben ser compatibles
		/// con este material.
		/// @note Los buffers de vértices que se enlacen después de este comando deben tener
		/// el tipo de vértice del material.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre El material debe tener registrado el renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @warning Para materiales de rasterizado, debe estar activo el renderpass
		/// con el que se vaya a renderizar usando este material.
		virtual void BindMaterial(Material* material) = 0;

		/// @brief Establece el vertex buffer que se va a usar en los próximos renderizados.
		/// 
		/// @note Los materiales que se usen después de este comando deben tener
		/// el mismo tipo de vértice.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void BindVertexBuffer(const IGpuVertexBuffer* buffer) = 0;

		/// @brief Establece el index buffer que se va a usar en los próximos renderizados.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void BindIndexBuffer(const IGpuIndexBuffer* buffer) = 0;

		/// <summary>
		/// Establece un material slot que estará asignado en los próximos comandos de renderizado.
		/// Pueden haber varios slots asignados en un mismo instante,
		/// siempre que no tengan el mismo id.
		/// </summary>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre El slot debe ser compatible con el material enlazado.
		/// @pre La lista de comandos debe estar abierta.
		virtual void BindMaterialSlot(const IMaterialSlot* slot) = 0;

		/// <summary>
		/// Envía datos push constant al shader.
		/// </summary>
		/// <param name="pushConstName">Nombre de los push const en el shader.</param>
		/// <param name="data">Información a enviar.</param>
		/// <param name="size">Número de bytes a enviar.</param>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre El push constant debe ser compatible con el material enlazado.
		/// @pre La lista de comandos debe estar abierta.
		void PushMaterialConstants(
			const std::string& pushConstName, 
			const void* data, 
			TSize size);

		/// <summary>
		/// Envía datos push constant al shader.
		/// </summary>
		/// <param name="pushConstName">Nombre de los push const en el shader.</param>
		/// <param name="data">Información a enviar.</param>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre El push constant debe ser compatible con el material enlazado.
		/// @pre La lista de comandos debe estar abierta.
		template <typename T> void PushMaterialConstants(const std::string& pushConstName, const T& data) {
			PushMaterialConstants(pushConstName, &data, sizeof(T));
		}

		/// <summary>
		/// Envía datos push constant al shader.
		/// </summary>
		/// <param name="pushConstName">Nombre de los push const en el shader.</param>
		/// <param name="data">Información a enviar.</param>
		/// <param name="size">Número de bytes a enviar.</param>
		/// <param name="offset">Offset, dentro de este push constant, con el que se mandarán datos.</param>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre El push constant debe ser compatible con el material enlazado.
		/// @pre La lista de comandos debe estar abierta.
		virtual void PushMaterialConstants(
			const std::string& pushConstName, 
			const void* data, 
			TSize size, 
			TSize offset) = 0;
		

		/// <summary>
		/// Renderiza los triángulos del vertex e index buffer,
		/// con el material enlazado.
		/// </summary>
		/// <param name="numIndices">Número de vértices/índices del vertex buffer.</param>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre Debe haber un buffer de vértices enlazado.
		/// @pre Debe haber un buffer de índices enlazado.
		/// @pre Debe estar establecido el viewport y el scissor.
		/// @pre Deben estar enlazados los slots y los push constants necesarios
		/// para rellenar el material.
		/// @pre La lista de comandos debe estar abierta.
		virtual void DrawSingleInstance(TSize numIndices) = 0;		
		
		/// <summary>
		/// Renderiza los triángulos del mesh,
		/// con el material enlazado.
		/// </summary>
		/// <param name="numIndices">Número de vértices/índices del mesh.</param>
		/// <param name="firstIndex">Primer vértice del mesh.</param>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre Debe haber un buffer de vértices enlazado.
		/// @pre Debe haber un buffer de índices enlazado.
		/// @pre Debe estar establecido el viewport y el scissor.
		/// @pre Deben estar enlazados los slots y los push constants necesarios
		/// para rellenar el material.
		/// @pre La lista de comandos debe estar abierta.
		virtual void DrawSingleMesh(TSize firstIndex, TSize numIndices) = 0;


		/// <summary>
		/// Ejecuta un trazado de rayos.
		/// </summary>
		/// <param name="raygenEntry">Índice del shader de generación de rayos que se va 
		/// a usar, según su posición en la tabla de shaders.</param>
		/// <param name="closestHitEntry">Índice del shader de colisión de rayos que se va 
		/// a usar, según su posición en la tabla de shaders.</param>
		/// <param name="missEntry">Índice del shader de fallo de rayos que se va 
		/// a usar, según su posición en la tabla de shaders.</param>
		/// <param name="resolution">Número de rayos generados en los ejes X e Y.</param>
		/// 
		/// @pre NO debe haber un renderpass activo.
		/// @pre Debe haber un material de trazado de rayos enlazado.
		/// @pre Deben estar enlazados los slots y los push constants necesarios
		/// para rellenar el material.
		/// @pre La lista de comandos debe estar abierta.
		/// @pre Debe ejecutarse en un equipo compatible con trazado de rayos.
		/// @pre Debe haberse activado la opción de trazado de rayos de OSKengine.
		/// 
		/// @todo Implementación en DX12.
		virtual void TraceRays(
			TSize raygenEntry, 
			TSize closestHitEntry, 
			TSize missEntry, 
			const Vector2ui& resolution) = 0;


		virtual void DispatchCompute(const Vector3ui& groupCount) = 0;
		virtual void BindComputePipeline(const IComputePipeline& computePipeline) = 0;


		/// <summary>
		/// Establece el viewport a renderizar.
		/// El viewport describe el área de la patnalla que se va a renderizar.
		/// Además, establece la profundida mínima y máxima.
		/// </summary>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void SetViewport(const Viewport& viewport) = 0;

		/// <summary>
		/// Establece qué área del renderizado será visible en la textura final.
		/// </summary>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void SetScissor(const Vector4ui& scissor) = 0;


		/// @brief Copia la imagen guardada en un buffer a una imagen en la GPU.
		/// Para poder subir una imagen a la GPU, primero debemos subir la
		/// imagen a un buffer intermedio, y después copiamos el contenido
		/// de ese buffer intermedio a la imagen final de la GPU.
		/// 
		/// @param source Buffer con los contenidos de la textura.
		/// @param dest Imagen final.
		/// @param layer Capa a la que se copiará los contenidos. Por defecto: 0.
		/// @param offset Offset, en bytes, respecto al inicio del buffer.
		/// 
		/// @pre El buffer de origen debe haber sido creado con GpuBufferUsage::TRANSFER_SOURCE.
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post El layout de la imagen después de efectuarse la copia segirá siendo GpuImageLayout::TRANSFER_DESTINATION.
		virtual void CopyBufferToImage(
			const GpuDataBuffer* source, 
			GpuImage* dest, 
			TSize layer = 0, 
			TSize offset = 0) = 0;

		/// @brief Copia el contenido de una imagen a otra.
		/// @param source Imagen con los contenidos a copiar.
		/// @param destination Destino de la copia.
		/// @param copyInfo Configuracióon de la copia.
		///
		/// @pre La imagen de origen debe haber sido creado con GpuImageUsage::TRANSFER_SOURCE.
		/// @pre La imagen de origen debe tener el layout GpuImageLayout::TRANSFER_SOURCE.
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post El layout de la imagen después de efectuarse la copia segirá siendo GpuImageLayout::TRANSFER_DESTINATION.
		virtual void CopyImageToImage(
			const GpuImage* source, 
			GpuImage* destination, 
			const CopyImageInfo& copyInfo) = 0;

		/// @brief Copia los contenidos del primer buffer al segundo.
		/// @param source Buffer donde están el contenido que se quiere copiar.
		/// @param dest Buffer en el que se va a poner el contenido copiado.
		/// @param size Tamaño, en bytes, del área de memoria que se va a copiar.
		/// @param sourceOffset Offset del área de memoria en el origen.
		/// @param destOffset Offset del área de memoria en el destino.
		virtual void CopyBufferToBuffer(
			const GpuDataBuffer* source,
			GpuDataBuffer* dest,
			TSize size,
			TSize sourceOffset,
			TSize destOffset) = 0;


		/// <summary>
		/// Registra un buffer intermedio.
		/// Como la operación de copia de datos de un buffer intermedio a un buffer final
		/// no se realiza hasta que se ejecute la lista de comandos, no podemos
		/// eliminar el buffer intermedio hasta la próxima ejecución de la
		/// lista de comandos.
		/// </summary>
		/// 
		/// @pre El buffer debe haber sido creado con GpuBufferUsage::TRANSFER_SOURCE.
		/// @pre El buffer debe haber sido creado con GpuSharedMemoryType::GPU_AND_CPU.
		/// @pre La lista de comandos debe estar abierta.
		void RegisterStagingBuffer(OwnedPtr<GpuDataBuffer> stagingBuffer);

		/// <summary>
		/// Elimina todos los buffers intermedios que ya no son necesarios.
		/// </summary>
		/// 
		/// @pre La lista debe haberse ejecutado antes de llamar a esta función.
		void DeleteAllStagingBuffers();

		void _SetSingleTimeUse();



		/// @brief Añade un marcador en la lista de comandos que podrá ser vista
		/// en un software de debbuging.
		/// @param mark Título del marcador.
		/// @param color Color del marcador.
		virtual void AddDebugMarker(const std::string& mark, const Color& color) = 0;

		/// @brief Comienza una sección de la lista de comandos que quedará marcada.
		/// @param mark Título de la marca.
		/// @param color Color de la marca.
		virtual void StartDebugSection(const std::string& mark, const Color& color) = 0;

		/// @brief Finaliza la sección actualmente marcada.
		virtual void EndDebugSection() = 0;


		TSize GetCommandListIndex() const;

	protected:

		/// <summary>
		/// Pipeline que está siendo grabada en un instante determinado.
		/// </summary>
		union {
			const IGraphicsPipeline* graphics;
			const IRaytracingPipeline* raytracing;
			const IComputePipeline* compute;
		} currentPipeline;

		/// <summary>
		/// Material que está siendo usado en un instante determinado.
		/// </summary>
		const Material* currentMaterial = nullptr;

		DynamicArray<RenderPassImageInfo> currentColorImages;
		RenderPassImageInfo currentDepthImage;
		RenderpassType currentRenderpassType = RenderpassType::INTERMEDIATE;

		bool isSingleUse = false;

	private:

		DynamicArray<UniquePtr<GpuDataBuffer>> stagingBuffersToDelete{};

	};

}
