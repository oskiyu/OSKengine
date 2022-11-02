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
		virtual void SetGpuImageBarrier(GpuImage* image, GpuImageLayout previousLayout, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageBarrierInfo& prevImageInfo = {}) = 0;

		/// <summary>
		/// Establece un barrier que sincroniza la ejecuci�n de comandos.
		/// Cambia el layout de la imagen.
		/// </summary>
		/// <param name="image">Imagen a la que se le cambiar� el layout.</param>
		/// <param name="nextLayout">Nuevo layout.</param>
		/// <param name="previous">Stage previo.</param>
		/// <param name="next">Stage siguiente.</param>
		/// <param name="imageInfo">Informaci�n sobre que subrecursos de la imagen ser�n afectados.</param>
		/// <param name="nextImageInfo"></param>
		/// 
		/// @note Se debe cambiar el layout de la imagen antes de ejecutar un comando sobre ella,
		/// si su layout actual no coincide con el necesario.
		/// 
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post Los subrecursos especificados por imageInfo estar�n en el nuevo layout.
		void SetGpuImageBarrier(GpuImage* image, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageBarrierInfo& prevImageInfo = {});


		/// <summary>
		/// Comienza el renderizado a un render target.
		/// </summary>
		/// <param name="color">Color con el que limpiar� la imagen.</param>
		/// 
		/// @note Se limpiar� la imagen con color negro.
		/// 
		/// @pre No debe haber ning�n renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		void BeginGraphicsRenderpass(RenderTarget* renderpass, const Color& color = Color::BLACK());

		virtual void BeginGraphicsRenderpass(DynamicArray<RenderPassImageInfo> colorImages, RenderPassImageInfo depthImage, const Color& color = Color::BLACK()) = 0;

		/// <summary>
		/// Finaliza el renderizado a un render target.
		/// </summary>
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

		/// <summary>
		/// Establece el vertex buffer que se va a usar en los pr�ximos renderizados.
		/// </summary>
		/// 
		/// @note Los materiales que se usen despu�s de este comando deben tener
		/// el mismo tipo de v�rtice.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void BindVertexBuffer(const IGpuVertexBuffer* buffer) = 0;

		/// <summary>
		/// Establece el index buffer que se va a usar en los pr�ximos renderizados.
		/// </summary>
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
		void PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size);

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
		virtual void PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) = 0;
		

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
		virtual void TraceRays(TSize raygenEntry, TSize closestHitEntry, TSize missEntry, const Vector2ui& resolution) = 0;


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

		/// <summary>
		/// Copia la imagen guardada en un buffer a una imagen en la GPU.
		/// Para poder subir una imagen a la GPU, primero debemos subir la
		/// imagen a un buffer intermedio, y despu�s copiamos el contenido
		/// de ese buffer intermedio a la imagen final de la GPU.
		/// </summary>
		/// <param name="source">Buffer con los contenidos de la textura.</param>
		/// <param name="dest">Imagen final.</param>
		/// 
		/// @pre El buffer de origen debe haber sido creado con GpuBufferUsage::TRANSFER_SOURCE.
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post El layout de la imagen despu�s de efectuarse la copia segir� siendo GpuImageLayout::TRANSFER_DESTINATION.
		virtual void CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest, TSize layer = 0, TSize offset = 0) = 0;

		/// <summary>
		/// Copia el contenido de una imagen a otra.
		/// </summary>
		/// <param name="source">Imagen con los contenidos a copiar.</param>
		/// <param name="destination">Destino de la copia.</param>
		/// 
		/// <param name="numLayers">N�mero de capas que se van a copiar.</param>
		/// <param name="srcStartLayer">Capa de origen a partir de la cual se copiar�.</param>
		/// <param name="dstStartLayer">Capa de destino a partir de la cual se escribir�.</param>
		/// 
		/// <param name="srcMipLevel">Mip level de la imagen de origen desde el cual se copiar�.</param>
		/// <param name="dstMipLevel">Mip level de la imagen de destino al que se copiar�.</param>
		/// 
		/// <param name="copySize">Regi�n de la imagen, en p�xeles, que se copiar�.
		/// Por defecto, se copia la imagen entera.</param>
		/// 
		/// @pre La imagen de origen debe haber sido creado con GpuImageUsage::TRANSFER_SOURCE.
		/// @pre La imagen de origen debe tener el layout GpuImageLayout::TRANSFER_SOURCE.
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post El layout de la imagen despu�s de efectuarse la copia segir� siendo GpuImageLayout::TRANSFER_DESTINATION.
		/// 
		/// @warning Si se copia con srcMipLevel != 0 y/o dstMipLevel != 0, se debe establecer un tama�o de la
		/// regi�n de copia v�lido (copySize).
		virtual void CopyImageToImage(const GpuImage* source, GpuImage* destination, TSize numLayers, TSize srcStartLayer, TSize dstStartLayer, TSize srcMipLevel, TSize dstMipLevel, Vector2ui copySize = Vector2ui(0u)) = 0;

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
