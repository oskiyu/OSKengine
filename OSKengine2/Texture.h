#pragma once

#include "IAsset.h"
#include "OwnedPtr.h"
#include "Vector2.hpp"

namespace OSK {

	class GpuImage;

	/// <summary>
	/// Una textura que será usada en renderizado 2D / 3D.
	/// </summary>
	class OSKAPI_CALL Texture : public IAsset {

	public:

		Texture(const std::string& assetFile);
		~Texture();

		OSK_ASSET_TYPE_REG("Texture");

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
		GpuImage* GetGpuImage() const;

		void _SetSize(const Vector2ui size);
		void _SetNumberOfChannels(TSize numChannels);
		void _SetImage(OwnedPtr<GpuImage> image);

	private:

		Vector2ui size;
		TSize numChannels = 0;

		OwnedPtr<GpuImage> image;

	};

}
