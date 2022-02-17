#pragma once

#include "OSKmacros.h"
#include "Vector4.hpp"
#include <type_traits>
#include "DynamicArray.hpp"
#include "OwnedPtr.h"

namespace OSK {

	class GpuImage;
	enum class GpuImageLayout;
	class IRenderpass;
	class Color;
	class IGraphicsPipeline;
	class IGpuVertexBuffer;
	class IGpuIndexBuffer;
	struct Viewport;
	class GpuDataBuffer;
	class IMaterialSlot;

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
		virtual void BindPipeline(IGraphicsPipeline* pipeline) = 0;

		/// <summary>
		/// Establece el vertex buffer que se va a usar en los pr�ximos renderizados.
		/// </summary>
		virtual void BindVertexBuffer(IGpuVertexBuffer* buffer) = 0;

		/// <summary>
		/// Establece el index buffer que se va a usar en los pr�ximos renderizados.
		/// </summary>
		virtual void BindIndexBuffer(IGpuIndexBuffer* buffer) = 0;

		/// <summary>
		/// Establece el descriptor que estar� en el slot 'index'.
		/// </summary>
		virtual void BindMaterialSlot(const IMaterialSlot* slot) = 0;


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



		void RegisterStagingBuffer(OwnedPtr<GpuDataBuffer> stagingBuffer);

		void DeleteAllStagingBuffers();

	private:

		DynamicArray<OwnedPtr<GpuDataBuffer>> stagingBuffersToDelete;

	};

}
