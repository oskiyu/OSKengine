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
	/// Formato de una textura.
	/// </summary>
	enum class TextureFormat {

		RGB_8bits,
		RGBA_8bits,

		INTERNAL_FONT

	};

	/// <summary>
	/// Devuelve el formato nativo, dado el formato de la textura.
	/// </summary>
	VkFormat ToNative(TextureFormat format);

	/// <summary>
	/// Devuelve el número de bytes por cada píxel.
	/// </summary>
	uint32_t GetNumberOfPixelsFromFormat(TextureFormat format);


	/// <summary>
	/// Representa un bitmap:
	/// una serie de bytes que representan una textura.
	/// </summary>
	struct Bitmap {

		/// <summary>
		/// Butes.
		/// </summary>
		std::vector<uint8_t> bytes;

		/// <summary>
		/// Tamaño de la textura, en píxeles.
		/// </summary>
		Vector2ui size;

		/// <summary>
		/// Formato de la textura.
		/// </summary>
		TextureFormat format;

	};

	/// <summary>
	/// Una textura 2D.
	/// </summary>
	struct OSKAPI_CALL Texture {

		friend class ContentManager;
		friend class OldMaterialInstance;
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
