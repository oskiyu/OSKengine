#pragma once

#include "IAsset.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "Vector2.hpp"
#include "IGpuImage.h"

namespace OSK::GRAPHICS {
	class IGpuImageView;
}

namespace OSK::ASSETS {

	/// <summary>
	/// Una textura que será usada en renderizado 2D / 3D.
	/// </summary>
	class OSKAPI_CALL Texture : public IAsset {

	public:

		Texture(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::Texture");

		/// <summary> Tamaño de la textura. </summary>
		/// 
		/// @note En píxeles.
		Vector2ui GetSize() const;

		/// <summary> Número de canales (red green blue, etc...). </summary>
		TSize GetNumberOfChannels() const;


		/// <summary>
		/// Imagen guardada en la GPU.
		/// Para renderizado.
		/// </summary>
		/// 
		/// @note No puede ser null.
		GRAPHICS::GpuImage* GetGpuImage() const;

		/// @return Image view para renderizado 2D, con un único nivel de mip.
		const GRAPHICS::IGpuImageView& GetTextureView2D() const;

		/// @return Image view para renderizado 3D, con varios niveles de mip.
		const GRAPHICS::IGpuImageView& GetTextureView() const;

		/// <summary> Establece el tamaño de la textura. </summary>
		/// 
		/// @warning Función interna: no llamar.
		/// @note En píxeles.
		void _SetSize(const Vector2ui size);

		/// <summary> Establece el número de canales (red green blue, etc...). </summary>
		/// 
		/// @warning Función interna: no llamar.
		void _SetNumberOfChannels(TSize numChannels);

		/// <summary> Establece la imagen GPU de la textura. </summary> </param>
		/// 
		/// @warning Función interna: no llamar.
		void _SetImage(OwnedPtr<GRAPHICS::GpuImage> image);

	private:

		/// <summary> @note En píxeles. </summary>
		Vector2ui size;
		TSize numChannels = 0;

		UniquePtr<GRAPHICS::GpuImage> image;

	};

}
