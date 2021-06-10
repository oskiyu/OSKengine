#pragma once

#include "GpuImage.h"
#include <vector>

#include "SharedPtr.hpp"

namespace OSK {

	/// <summary>
	/// 'Filtro' de la textura.
	/// </summary>
	enum class TextureFilterType {

		/// <summary>
		/// Suavizado.
		/// </summary>
		LINEAR,

		/// <summary>
		/// Pixelado.
		/// </summary>
		NEAREST

	};

	/// <summary>
	/// Una textura 2D.
	/// </summary>
	struct OSKAPI_CALL Texture {

		friend class ContentManager;
		friend class MaterialInstance;
		friend class ShadowMap;
		friend class RenderizableScene;
		friend class RenderAPI;
		friend class RenderTarget;

		Texture() {
			image = new VULKAN::GpuImage;
		}

		/// <summary>
		/// Tamaño de la textura.
		/// </summary>
		Vector2ui size;

	private:

		/// <summary>
		/// Imagen (en la GPU) de la textura).
		/// </summary>
		SharedPtr<VULKAN::GpuImage> image;

	};

}
