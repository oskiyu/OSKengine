#pragma once

#include "OSKmacros.h"
#include "Vector4.hpp"
#include <type_traits>
#include "DynamicArray.hpp"
#include "OwnedPtr.h"
#include "IGpuDataBuffer.h"
#include <string>

namespace OSK {
	class Color;
}

namespace OSK::GRAPHICS {

	class Material;
	class IMaterialSlot;
	class GpuImage;
	enum class GpuImageLayout;
	class IRenderpass;
	class IGpuVertexBuffer;
	class IGpuIndexBuffer;
	struct Viewport;
	class IGraphicsPipeline;
	class IRaytracingPipeline;

	/// <summary>
	/// Una lista de comandos contiene una serie de comandos que ser�n
	/// enviados a la GPU para su ejecuci�n.
	/// 
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecuci�n.
	/// </summary>
	/// 
	/// @note Los comandos ser�n ejecutados en orden una vez se env�e la lista
	/// a la GPU.
	class OSKAPI_CALL ICommandList {

	public:

		virtual ~ICommandList();

		template <typename T> T* As() const requires std::is_base_of_v<ICommandList, T> {
			return (T*)this;
		}

		/// <summary>
		/// Vac�a la lista de comandos.
		/// 
		/// @note Debe llamarse antes de comenzar a grabar comandos.
		/// </summary>
		/// 
		/// @pre Debe estar cerrado.
		virtual void Reset() = 0;

		/// <summary>
		/// Comienza a grabar comandos en la lista.
		/// </summary>
		/// 
		/// @pre Debe estar cerrado.
		virtual void Start() = 0;

		/// <summary>
		/// Cierra la lista de comandos, y la deja lista para ser enviada
		/// a la GPU.
		/// </summary>
		/// 
		/// @pre Debe estar abierto.
		virtual void Close() = 0;

		/// <summary>
		/// Cambia el layout interno de la imagen en la memoria de la GPU.
		/// </summary>
		/// <param name="next">Layout en el que estar� la imagen.</param>
		/// <param name="baseLayer">Primera capa a partir de la cual se cambiar�n los layouts.</param>
		/// <param name="numLayers">N�mero de capas a cambiar.</param>
		/// 
		/// @note Se debe cambiar el layout de la imagen antes de ejecutar un comando sobre ella,
		/// si su layout actual no coincide con el necesario.
		/// 
		/// @note Se considerar� el layout anterior el layout que ten�a la imagen antes de este comando.
		/// 
		/// @pre El n�mero de capas (numLayers) debe ser mayor que 0.
		/// @pre La lista de comandos debe estar abierta.
		void TransitionImageLayout(GpuImage* image, GpuImageLayout next, TSize baseLayer, TSize numLayers);

		/// <summary>
		/// Cambia el layout interno de la imagen en la memoria de la GPU.
		/// </summary>
		/// <param name="next">Layout en el que estar� la imagen.</param>
		/// <param name="baseLayer">Primera capa a partir de la cual se cambiar�n los layouts.</param>
		/// <param name="numLayers">N�mero de capas a cambiar.</param>
		/// 
		/// @note Se debe cambiar el layout de la imagen antes de ejecutar un comando sobre ella,
		/// si su layout actual no coincide con el necesario.
		/// 
		/// @pre El n�mero de capas (numLayers) debe ser mayor que 0.
		/// @pre La lista de comandos debe estar abierta.
		virtual void TransitionImageLayout(GpuImage* image, GpuImageLayout previous, GpuImageLayout next, TSize baseLayer, TSize numLayers) = 0;


		/// <summary>
		/// Comienza el renderizado a un renderpass.
		/// </summary>
		/// 
		/// @note Se limpiar� la imagen con color negro.
		/// 
		/// @pre No debe haber ning�n renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void BeginRenderpass(IRenderpass* renderpass) = 0;

		/// <summary>
		/// Comienza el renderizado a un renderpass.
		/// </summary>
		/// 
		/// <param name="color">Color con el que limpiar� la imagen.</param>
		/// 
		/// @pre No debe haber ning�n renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void BeginAndClearRenderpass(IRenderpass* renderpass, const Color& color) = 0;

		/// <summary>
		/// Finaliza el renderizado a un renderpass.
		/// </summary>
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void EndRenderpass(IRenderpass* renderpass) = 0;


		/// <summary>
		/// Establece el material que se va a usar a la hora de renderizar los pr�ximos comandos.
		/// </summary>
		/// 
		/// @note Los slots que se enlacen despu�s de este comando deben ser compatibles
		/// con este material.
		/// @note Los buffers de v�rtices que se enlacen despu�s de este comando deben tener
		/// el tipo de v�rtice del material.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre El material debe tener registrado el renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		virtual void BindMaterial(const Material* material) = 0;

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

		virtual void TraceRays(TSize raygenEntry, TSize closestHitEntry, TSize missEntry, const Vector2ui& resolution) = 0;


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
		/// @note El layout de la imagen despu�s de efectuarse la copia segir� siendo GpuImageLayout::TRANSFER_DESTINATION.
		virtual void CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest, TSize layer = 0, TSize offset = 0) = 0;

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
		const IGraphicsPipeline* currentPipeline = nullptr;
		const IRaytracingPipeline* currentRtPipeline = nullptr;

		/// <summary>
		/// Material que est� siendo usado en un instante determinado.
		/// </summary>
		const Material* currentMaterial = nullptr;

		/// <summary>
		/// Renderepass que est� siendo grabado en un instante determinado.
		/// </summary>
		const IRenderpass* currentRenderpass = nullptr;

		bool isSingleUse = false;

	private:

		DynamicArray<OwnedPtr<GpuDataBuffer>> stagingBuffersToDelete{};

	};

}
