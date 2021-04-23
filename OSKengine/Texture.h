#pragma once

#include "GPUImage.h"
#include <vector>

namespace OSK {

	/// <summary>
	/// Una textura 2D.
	/// </summary>
	struct OSKAPI_CALL Texture {

		/// <summary>
		/// Elimina la textura.
		/// </summary>
		~Texture() {
			Image.Destroy();
		}

		/// <summary>
		/// Tamaño de la textura.
		/// </summary>
		Vector2ui Size;

		/// <summary>
		/// Imagen (en la GPU) de la textura).
		/// </summary>
		VULKAN::GPUImage Image;

	};

}
