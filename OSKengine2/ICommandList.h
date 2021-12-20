#pragma once

#include "OSKmacros.h"
#include <type_traits>

namespace OSK {

	class GpuImage;
	enum class GpuImageLayout;
	class IRenderpass;
	class Color;

	/// <summary>
	/// Una lista de comandos contiene una serie de comandos que serán
	/// enviados a la GPU para su ejecución.
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecución.
	/// </summary>
	class OSKAPI_CALL ICommandList {

	public:

		virtual ~ICommandList() = default;

		template <typename T> T* As() const requires std::is_base_of_v<ICommandList, T> {
			return (T*)this;
		}

		/// <summary>
		/// Vacía la lista de comandos.
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
		/// Implementación en las Render Apis High Profile.
		/// </summary>
		/// <param name="next">Layout en el que estará la imagen.</param>
		virtual void TransitionImageLayout(GpuImage* image, GpuImageLayout next) = 0;


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

	};

}
