#pragma once

#include "IAsset.h"
#include "OwnedPtr.h"
#include "Vector2.hpp"
#include "IGpuImage.h"

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
		/// Tamaño de la textura.
		/// 
		/// @note En píxeles.
		/// </summary>
		Vector2ui GetSize() const;

		/// <summary>
		/// Número de canales (red green blue, etc...).
		/// </summary>
		TSize GetNumberOfChannels() const;

		/// <summary>
		/// Imagen guardada en la GPU.
		/// Para renderizado.
		/// 
		/// @note No puede ser null.
		/// </summary>
		GRAPHICS::GpuImage* GetGpuImage() const;

		/// <summary>
		/// Establece el tamaño de la textura.
		/// </summary>
		/// 
		/// @warning Función interna: no llamar.
		/// 
		/// @note En píxeles.
		void _SetSize(const Vector2ui size);

		/// <summary>
		/// Establece el número de canales..
		/// </summary>
		/// 
		/// @warning Función interna: no llamar.
		void _SetNumberOfChannels(TSize numChannels);

		/// <summary>
		/// Establece la imagen de la textura.
		/// </summary>
		/// 
		/// @warning Función interna: no llamar.
		/// </param>
		void _SetImage(OwnedPtr<GRAPHICS::GpuImage> image);

	private:

		Vector2ui size;
		TSize numChannels = 0;

		OwnedPtr<GRAPHICS::GpuImage> image;

	};

}
