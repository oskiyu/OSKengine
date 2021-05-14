#pragma once

#include "GPUImage.h"
#include <vector>

namespace OSK {

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

		/// <summary>
		/// Elimina la textura.
		/// </summary>
		~Texture() {
			image.Destroy();
		}

		/// <summary>
		/// Tamaño de la textura.
		/// </summary>
		Vector2ui size;

	private:

		/// <summary>
		/// Imagen (en la GPU) de la textura).
		/// </summary>
		VULKAN::GPUImage image;

	};

}
