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

	/// @brief Par�metros para la copia de una imagen a otra
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

		/// @brief Localizaci�n del �rea copiada de la imagen
		/// de origen. Esquina superior izquierda.
		Vector3ui sourceOffset = 0;

		/// @brief Localizaci�n del �rea copiada de la imagen
		/// final. Esquina superior izquierda.
		Vector3ui destinationOffset = 0;

		/// @brief �rea copiada.
		Vector3ui copySize = 0;


		/// @brief Capa de origen a partir de la cual se copiar�.
		TIndex sourceArrayLevel = 0;

		/// @brief Capa de destino a partir de la cual se escribir�.
		TIndex destinationArrayLevel = 0;

		/// @brief N�mero de capas que se van a copiar.
		/// Para copiar todas las capas, establecer como ALL_ARRAY_LEVELS.
		TSize numArrayLevels = 1;


		/// @brief Mip level de la imagen de origen desde el cual se copiar�.
		TIndex sourceMipLevel = 0;

		/// @brief Mip level de la imagen de destino al que se copiar�.
		TIndex destinationMipLevel = 0;

		const static TIndex ALL_ARRAY_LEVELS = 0;

	};

	/// @brief Una lista de comandos contiene una serie de comandos que ser�n
	/// enviados a la GPU para su ejecuci�n.
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecuci�n.
	/// 
	/// @note Los comandos ser�n ejecutados en orden una vez se env�e la lista
	/// a la GPU.
	class OSKAPI_CALL ICommandList : public IGpuObject {

	public:

		virtual ~ICommandList();

		OSK_DEFINE_AS(ICommandList);

		/// @brief Vac�a la lista de comandos.
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


		/// @brief Establece un barrier que sincroniza la ejecuci�n de comandos.
		/// Tambi�n cambia el layout de la imagen.
		/// 
		/// @see GpuImageBarrierInfo.
		/// 
		/// @param image Imagen a la que se le cambiar� el layout.
		/// @param previousLayout Layout anterior.
		/// @param nextLayout Nuevo layout.
		/// @param previous Stage previo.
		/// @param next Stage siguiente.
		/// @param prevImageInfo Informaci�n sobre que subrecursos de la imagen ser�n afectados.
		///
		/// @note Se debe cambiar el layout de la imagen antes de ejecutar un comando sobre ella,
		/// si su layout actual no coincide con el necesario.
		/// 
		/// @pre La lista de comandos debe estar abierta.
		/// @pre previousLayout debe ser el layout de la imagen antes de este comando.
		/// 
		/// @post Los subrecursos especificados por imageInfo estar�n en el nuevo layout.
		virtual void SetGpuImageBarrier(
			GpuImage* image, 
			GpuImageLayout previousLayout, 
			GpuImageLayout nextLayout, 
			GpuBarrierInfo previous, 
			GpuBarrierInfo next, 
			const GpuImageBarrierInfo& prevImageInfo = {}) = 0;

		/// @brief Establece un barrier que sincroniza la ejecuci�n de comandos.
		/// Cambia el layout de la imagen.
		/// 
		/// @param image Imagen a la que se le cambiar� el layout.
		/// @param nextLayout Nuevo layout.
		/// @param previous Stage previo.
		/// @param next Stage siguiente.
		/// @param prevImageInfo Informaci�n sobre que subrecursos de la imagen ser�n afectados.
		/// 
		/// @note Se debe cambiar el layout de la imagen antes de ejecutar un comando sobre ella,
		/// si su layout actual no coincide con el necesario.
		/// 
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post Los subrecursos especificados por imageInfo estar�n en el nuevo layout.
		void SetGpuImageBarrier(
			GpuImage* image, 
			GpuImageLayout nextLayout, 
			GpuBarrierInfo previous, 
			GpuBarrierInfo next, 
			const GpuImageBarrierInfo& prevImageInfo = {});


		/// @brief Comienza el renderizado a un render target.
		/// @param renderTarget Render target sobre el que se renderizar�.
		/// @param color Color con el que limpiar� la imagen.
		/// 
		/// @note Por defecto, se limpiar� la imagen con color negro.
		/// @note Se encarga de establecer el layout de las im�genes de color y de profundidad.
		/// 
		/// @pre No debe haber ning�n renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post Las im�genes de color se encontrar�n en GpuImageLayout::COLOR_ATTACHMENT.
		/// @post La imagen de prfundidad se encontrar�n en GpuImageLayout::DEPTH_STENCIL_TARGET.
		void BeginGraphicsRenderpass(
			RenderTarget* renderTarget, 
			const Color& color = Color::BLACK());

		/// @brief Comienza el renderizado a las im�genes dadas.
		/// @param colorImages Im�genes de color sobre las que se renderizar�.
		/// @param depthImage Imagen de profundidad.
		/// @param color Color con el que limpiar� la imagen.
		/// 
		/// @note Por defecto, se limpiar� la imagen con color negro.
		/// @note Se encarga de establecer el layout de las im�genes de color y de profundidad.
		/// 
		/// @pre No debe haber ning�n renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post Las im�genes de color se encontrar�n en GpuImageLayout::COLOR_ATTACHMENT.
		/// @post La imagen de prfundidad se encontrar�n en GpuImageLayout::DEPTH_STENCIL_TARGET.
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


		/// @brief Establece el material que se va a usar a la hora de renderizar los pr�ximos comandos.
		///
		/// @note Los slots que se enlacen despu�s de este comando deben ser compatibles
		/// con este material.
		/// @note Los buffers de v�rtices que se enlacen despu�s de este comando deben tener
		/// el tipo de v�rtice del material.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre El material debe tener registrado el renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @warning Para materiales de rasterizado, debe estar activo el renderpass
		/// con el que se vaya a renderizar usando este material.
		virtual void BindMaterial(Material* material) = 0;

		/// @brief Establece el vertex buffer que se va a usar en los pr�ximos renderizados.
		/// 
		/// @note Los materiales que se usen despu�s de este comando deben tener
		/// el mismo tipo de v�rtice.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void BindVertexBuffer(const IGpuVertexBuffer* buffer) = 0;

		/// @brief Establece el index buffer que se va a usar en los pr�ximos renderizados.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void BindIndexBuffer(const IGpuIndexBuffer* buffer) = 0;

		/// <summary>
		/// Establece un material slot que estar� asignado en los pr�ximos comandos de renderizado.
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
		/// Env�a datos push constant al shader.
		/// </summary>
		/// <param name="pushConstName">Nombre de los push const en el shader.</param>
		/// <param name="data">Informaci�n a enviar.</param>
		/// <param name="size">N�mero de bytes a enviar.</param>
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
		/// Env�a datos push constant al shader.
		/// </summary>
		/// <param name="pushConstName">Nombre de los push const en el shader.</param>
		/// <param name="data">Informaci�n a enviar.</param>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre El push constant debe ser compatible con el material enlazado.
		/// @pre La lista de comandos debe estar abierta.
		template <typename T> void PushMaterialConstants(const std::string& pushConstName, const T& data) {
			PushMaterialConstants(pushConstName, &data, sizeof(T));
		}

		/// <summary>
		/// Env�a datos push constant al shader.
		/// </summary>
		/// <param name="pushConstName">Nombre de los push const en el shader.</param>
		/// <param name="data">Informaci�n a enviar.</param>
		/// <param name="size">N�mero de bytes a enviar.</param>
		/// <param name="offset">Offset, dentro de este push constant, con el que se mandar�n datos.</param>
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
		/// Renderiza los tri�ngulos del vertex e index buffer,
		/// con el material enlazado.
		/// </summary>
		/// <param name="numIndices">N�mero de v�rtices/�ndices del vertex buffer.</param>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre Debe haber un buffer de v�rtices enlazado.
		/// @pre Debe haber un buffer de �ndices enlazado.
		/// @pre Debe estar establecido el viewport y el scissor.
		/// @pre Deben estar enlazados los slots y los push constants necesarios
		/// para rellenar el material.
		/// @pre La lista de comandos debe estar abierta.
		virtual void DrawSingleInstance(TSize numIndices) = 0;		
		
		/// <summary>
		/// Renderiza los tri�ngulos del mesh,
		/// con el material enlazado.
		/// </summary>
		/// <param name="numIndices">N�mero de v�rtices/�ndices del mesh.</param>
		/// <param name="firstIndex">Primer v�rtice del mesh.</param>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre Debe haber un buffer de v�rtices enlazado.
		/// @pre Debe haber un buffer de �ndices enlazado.
		/// @pre Debe estar establecido el viewport y el scissor.
		/// @pre Deben estar enlazados los slots y los push constants necesarios
		/// para rellenar el material.
		/// @pre La lista de comandos debe estar abierta.
		virtual void DrawSingleMesh(TSize firstIndex, TSize numIndices) = 0;


		/// <summary>
		/// Ejecuta un trazado de rayos.
		/// </summary>
		/// <param name="raygenEntry">�ndice del shader de generaci�n de rayos que se va 
		/// a usar, seg�n su posici�n en la tabla de shaders.</param>
		/// <param name="closestHitEntry">�ndice del shader de colisi�n de rayos que se va 
		/// a usar, seg�n su posici�n en la tabla de shaders.</param>
		/// <param name="missEntry">�ndice del shader de fallo de rayos que se va 
		/// a usar, seg�n su posici�n en la tabla de shaders.</param>
		/// <param name="resolution">N�mero de rayos generados en los ejes X e Y.</param>
		/// 
		/// @pre NO debe haber un renderpass activo.
		/// @pre Debe haber un material de trazado de rayos enlazado.
		/// @pre Deben estar enlazados los slots y los push constants necesarios
		/// para rellenar el material.
		/// @pre La lista de comandos debe estar abierta.
		/// @pre Debe ejecutarse en un equipo compatible con trazado de rayos.
		/// @pre Debe haberse activado la opci�n de trazado de rayos de OSKengine.
		/// 
		/// @todo Implementaci�n en DX12.
		virtual void TraceRays(
			TSize raygenEntry, 
			TSize closestHitEntry, 
			TSize missEntry, 
			const Vector2ui& resolution) = 0;


		virtual void DispatchCompute(const Vector3ui& groupCount) = 0;
		virtual void BindComputePipeline(const IComputePipeline& computePipeline) = 0;


		/// <summary>
		/// Establece el viewport a renderizar.
		/// El viewport describe el �rea de la patnalla que se va a renderizar.
		/// Adem�s, establece la profundida m�nima y m�xima.
		/// </summary>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void SetViewport(const Viewport& viewport) = 0;

		/// <summary>
		/// Establece qu� �rea del renderizado ser� visible en la textura final.
		/// </summary>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void SetScissor(const Vector4ui& scissor) = 0;


		/// @brief Copia la imagen guardada en un buffer a una imagen en la GPU.
		/// Para poder subir una imagen a la GPU, primero debemos subir la
		/// imagen a un buffer intermedio, y despu�s copiamos el contenido
		/// de ese buffer intermedio a la imagen final de la GPU.
		/// 
		/// @param source Buffer con los contenidos de la textura.
		/// @param dest Imagen final.
		/// @param layer Capa a la que se copiar� los contenidos. Por defecto: 0.
		/// @param offset Offset, en bytes, respecto al inicio del buffer.
		/// 
		/// @pre El buffer de origen debe haber sido creado con GpuBufferUsage::TRANSFER_SOURCE.
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post El layout de la imagen despu�s de efectuarse la copia segir� siendo GpuImageLayout::TRANSFER_DESTINATION.
		virtual void CopyBufferToImage(
			const GpuDataBuffer* source, 
			GpuImage* dest, 
			TSize layer = 0, 
			TSize offset = 0) = 0;

		/// @brief Copia el contenido de una imagen a otra.
		/// @param source Imagen con los contenidos a copiar.
		/// @param destination Destino de la copia.
		/// @param copyInfo Configuraci�on de la copia.
		///
		/// @pre La imagen de origen debe haber sido creado con GpuImageUsage::TRANSFER_SOURCE.
		/// @pre La imagen de origen debe tener el layout GpuImageLayout::TRANSFER_SOURCE.
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post El layout de la imagen despu�s de efectuarse la copia segir� siendo GpuImageLayout::TRANSFER_DESTINATION.
		virtual void CopyImageToImage(
			const GpuImage* source, 
			GpuImage* destination, 
			const CopyImageInfo& copyInfo) = 0;

		/// @brief Copia los contenidos del primer buffer al segundo.
		/// @param source Buffer donde est�n el contenido que se quiere copiar.
		/// @param dest Buffer en el que se va a poner el contenido copiado.
		/// @param size Tama�o, en bytes, del �rea de memoria que se va a copiar.
		/// @param sourceOffset Offset del �rea de memoria en el origen.
		/// @param destOffset Offset del �rea de memoria en el destino.
		virtual void CopyBufferToBuffer(
			const GpuDataBuffer* source,
			GpuDataBuffer* dest,
			TSize size,
			TSize sourceOffset,
			TSize destOffset) = 0;


		/// <summary>
		/// Registra un buffer intermedio.
		/// Como la operaci�n de copia de datos de un buffer intermedio a un buffer final
		/// no se realiza hasta que se ejecute la lista de comandos, no podemos
		/// eliminar el buffer intermedio hasta la pr�xima ejecuci�n de la
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
		/// @pre La lista debe haberse ejecutado antes de llamar a esta funci�n.
		void DeleteAllStagingBuffers();

		void _SetSingleTimeUse();



		/// @brief A�ade un marcador en la lista de comandos que podr� ser vista
		/// en un software de debbuging.
		/// @param mark T�tulo del marcador.
		/// @param color Color del marcador.
		virtual void AddDebugMarker(const std::string& mark, const Color& color) = 0;

		/// @brief Comienza una secci�n de la lista de comandos que quedar� marcada.
		/// @param mark T�tulo de la marca.
		/// @param color Color de la marca.
		virtual void StartDebugSection(const std::string& mark, const Color& color) = 0;

		/// @brief Finaliza la secci�n actualmente marcada.
		virtual void EndDebugSection() = 0;


		TSize GetCommandListIndex() const;

	protected:

		/// <summary>
		/// Pipeline que est� siendo grabada en un instante determinado.
		/// </summary>
		union {
			const IGraphicsPipeline* graphics;
			const IRaytracingPipeline* raytracing;
			const IComputePipeline* compute;
		} currentPipeline;

		/// <summary>
		/// Material que est� siendo usado en un instante determinado.
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
