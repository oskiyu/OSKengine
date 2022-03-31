#pragma once

#include "OSKmacros.h"
#include "Vector4.hpp"
#include <type_traits>
#include "DynamicArray.hpp"
#include "OwnedPtr.h"
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
	class GpuDataBuffer;
	class IGraphicsPipeline;

	/// <summary>
	/// Una lista de comandos contiene una serie de comandos que ser�n
	/// enviados a la GPU para su ejecuci�n.
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecuci�n.
	/// </summary>
	class OSKAPI_CALL ICommandList {

	public:

		virtual ~ICommandList();

		template <typename T> T* As() const requires std::is_base_of_v<ICommandList, T> {
			return (T*)this;
		}

		/// <summary>
		/// Vac�a la lista de comandos.
		/// 
		/// Debe llamarse una vez cada frame, antes de comenzar a grabar
		/// comandos.
		/// </summary>
		virtual void Reset() = 0;

		/// <summary>
		/// Comienza a grabar comandos en la lista.
		/// </summary>
		virtual void Start() = 0;

		/// <summary>
		/// Cierra la lista de comandos, y la deja lista para ser enviada
		/// a la GPU.
		/// </summary>
		virtual void Close() = 0;

		/// <summary>
		/// Cambia el layout interno de la imagen en la memoria de la GPU.
		/// 
		/// Implementaci�n en las Render Apis High Profile.
		/// </summary>
		/// <param name="next">Layout en el que estar� la imagen.</param>
		void TransitionImageLayout(GpuImage* image, GpuImageLayout next);

		/// <summary>
		/// Cambia el layout interno de la imagen en la memoria de la GPU.
		/// 
		/// Implementaci�n en las Render Apis High Profile.
		/// </summary>
		/// <param name="next">Layout en el que estar� la imagen.</param>
		virtual void TransitionImageLayout(GpuImage* image, GpuImageLayout previous, GpuImageLayout next) = 0;


		/// <summary>
		/// Comienza el renderizado a un renderpass.
		/// </summary>
		virtual void BeginRenderpass(IRenderpass* renderpass) = 0;

		/// <summary>
		/// Comienza el renderizado a un renderpass.
		/// </summary>
		virtual void BeginAndClearRenderpass(IRenderpass* renderpass, const Color& color) = 0;

		/// <summary>
		/// Finaliza el renderizado a un renderpass.
		/// </summary>
		virtual void EndRenderpass(IRenderpass* renderpass) = 0;


		/// <summary>
		/// Establece el pipeline que se va a usar a la hora de renderizar los pr�ximos comandos.
		/// </summary>
		virtual void BindMaterial(const Material* material) = 0;

		/// <summary>
		/// Establece el vertex buffer que se va a usar en los pr�ximos renderizados.
		/// </summary>
		virtual void BindVertexBuffer(IGpuVertexBuffer* buffer) = 0;

		/// <summary>
		/// Establece el index buffer que se va a usar en los pr�ximos renderizados.
		/// </summary>
		virtual void BindIndexBuffer(IGpuIndexBuffer* buffer) = 0;

		/// <summary>
		/// Establece un material slot que estar� asignado en los pr�ximos comandos de renderizado.
		/// Pueden haber varios slots asignados en un mismo instante,
		/// siempre que no tengan el mismo id.
		/// </summary>
		virtual void BindMaterialSlot(const IMaterialSlot* slot) = 0;

		/// <summary>
		/// Env�a datos push constant al shader.
		/// </summary>
		/// <param name="pushConstName">Nombre de los push const en el shader.</param>
		/// <param name="data">Informaci�n a enviar.</param>
		/// <param name="size">N�mero de bytes a enviar.</param>
		void PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size);

		/// <summary>
		/// Env�a datos push constant al shader.
		/// </summary>
		/// <param name="pushConstName">Nombre de los push const en el shader.</param>
		/// <param name="data">Informaci�n a enviar.</param>
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
		virtual void PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) = 0;
		

		/// <summary>
		/// Renderiza los tri�ngulos del vertex e index buffer,
		/// con el material enlazado.
		/// </summary>
		/// <param name="numIndices">N�mero de v�rtices/�ndices del vertex buffer.</param>
		virtual void DrawSingleInstance(TSize numIndices) = 0;		
		
		/// <summary>
		/// Renderiza los tri�ngulos del mesh,
		/// con el material enlazado.
		/// </summary>
		/// <param name="numIndices">N�mero de v�rtices/�ndices del mesh.</param>
		/// <param name="firstIndex">Primer v�rtice del mesh.</param>
		virtual void DrawSingleMesh(TSize firstIndex, TSize numIndices) = 0;


		/// <summary>
		/// Establece el viewport a renderizar.
		/// El viewport describe el �rea de la pantalla que se va a renderizar.
		/// Adem�s, establece la profundida m�nima y m�xima.
		/// </summary>
		virtual void SetViewport(const Viewport& viewport) = 0;

		/// <summary>
		/// Establece qu� �rea del renderizado ser� visible en la textura final.
		/// </summary>
		virtual void SetScissor(const Vector4ui& scissor) = 0;

		/// <summary>
		/// Copia la imagen guardada en un buffer a una imagen en la GPU.
		/// Para poder subir una imagen a la GPU, primero debemos subir la
		/// imagen a un buffer intermedio, y despu�s copiamos el contenido
		/// de ese buffer intermedio a la imagen final de la GPU.
		/// </summary>
		/// <param name="source">Buffer con los contenidos de la textura.</param>
		/// <param name="dest">Imagen final.</param>
		virtual void CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest) = 0;


		/// <summary>
		/// Registra un buffer intermedio.
		/// Como la operaci�n de copia de datos de un buffer intermedio a un buffer final
		/// no se realiza hasta que se ejecute la lista de comandos, no podemos
		/// eliminar el buffer intermedio hasta la pr�xima ejecuci�n de la
		/// lista de comandos.
		/// </summary>
		void RegisterStagingBuffer(OwnedPtr<GpuDataBuffer> stagingBuffer);

		/// <summary>
		/// Elimina todos los buffers intermedios que ya no son necesarios.
		/// </summary>
		void DeleteAllStagingBuffers();

	protected:

		/// <summary>
		/// Pipeline que est� siendo grabada en un instante determinado.
		/// </summary>
		const IGraphicsPipeline* currentPipeline = nullptr;

		/// <summary>
		/// Material que est� siendo usado en un instante determinado.
		/// </summary>
		const Material* currentMaterial = nullptr;

		/// <summary>
		/// Renderepass que est� siendo grabado en un instante determinado.
		/// </summary>
		const IRenderpass* currentRenderpass = nullptr;

	private:

		DynamicArray<OwnedPtr<GpuDataBuffer>> stagingBuffersToDelete;

	};

}
