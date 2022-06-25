#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"

namespace OSK::GRAPHICS {

	enum class RenderpassType;

	/// <summary>
	/// Un renderpass representa una fase de renderizado.
	/// Todas las operaciones de renderizado se aplicar�n sobre la imagen
	/// que defina el renderpass enlazado.
	/// Cuenta con las im�genes donde se renderizar� la escena.
	/// </summary>
	class OSKAPI_CALL IRenderpass {

	public:

		virtual ~IRenderpass() = default;

		/// <summary>
		/// Devuelve la imagen con el �ndice dado.
		/// </summary>
		/// <param name="index"></param>
		/// 
		/// @throws std::runtime_error Si el �ndice no pertenece a ninguna imagen.
		GpuImage* GetImage(TSize index) const;
		TSize GetNumberOfImages() const;

		/// <summary>
		/// Establece las im�genes del swapchain donde se va a renderizar (para pantalla).
		/// </summary>
		virtual void SetImages(GpuImage* image0, GpuImage* image1, GpuImage* image2) = 0;

		/// <summary>
		/// Comprueba si un renderpass es un renderpass intermedio o el
		/// renderpass final que renderiza la imagen que se mostrar� en
		/// el monitor.
		/// </summary>
		RenderpassType GetType() const;

		template <typename T> T* As() const requires std::is_base_of_v<IRenderpass, T> {
			return (T*)this;
		}

	protected:

		IRenderpass(RenderpassType mode);

		RenderpassType type;
		GpuImage* images[3];
		UniquePtr<GpuImage> colorImgs[3];
		UniquePtr<GpuImage> depthImgs[3];

	};

}
