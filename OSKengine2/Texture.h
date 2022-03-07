#pragma once

#include "IAsset.h"
#include "OwnedPtr.h"
#include "Vector2.hpp"

namespace OSK::GRAPHICS {
	class GpuImage;
}

namespace OSK::ASSETS {

	/// <summary>
	/// Una textura que será usada en renderizado 2D / 3D.
	/// </summary>
	class OSKAPI_CALL Texture : public IAsset {

	public:

		Texture(const std::string& assetFile);
		~Texture();

		OSK_ASSET_TYPE_REG("OSK::Texture");

		/// <summary>
		/// Tamaño, en píxeles, de la textura.
		/// </summary>
		Vector2ui GetSize() const;

		/// <summary>
		/// Número de canales (red green blue, etc...).
		/// </summary>
		TSize GetNumberOfChannels() const;

		/// <summary>
		/// Imagen guardada en la GPU.
		/// Para renderizado.
		/// </summary>
		GRAPHICS::GpuImage* GetGpuImage() const;

		void _SetSize(const Vector2ui size);
		void _SetNumberOfChannels(TSize numChannels);
		void _SetImage(OwnedPtr<GRAPHICS::GpuImage> image);

	private:

		Vector2ui size;
		TSize numChannels = 0;

		OwnedPtr<GRAPHICS::GpuImage> image;

	};

}
