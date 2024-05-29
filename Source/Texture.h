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

	/// @brief Una textura que ser� usada en renderizado 2D / 3D.
	class OSKAPI_CALL Texture : public IAsset {

	public:

		Texture(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::Texture");


		/// @return Tama�o de la textura.
		/// @note En p�xeles.
		Vector2ui GetSize() const;

		/// @return N�mero de canales (red green blue, etc...).
		USize32 GetNumberOfChannels() const;


		/// @return Imagen guardada en la GPU.
		/// @note No puede ser null.
		/// @stablepointer
		const GRAPHICS::GpuImage* GetGpuImage() const;

		/// @return Image view para renderizado 2D, con un �nico nivel de mip.
		/// @stablepointer
		const GRAPHICS::IGpuImageView& GetTextureView2D() const;

		/// @return Image view para renderizado 3D, con varios niveles de mip.
		/// @stablepointer
		const GRAPHICS::IGpuImageView& GetTextureView() const;


		/// @brief Establece el tama�o de la textura.
		/// @param size Tama�o, en p�xeles.
		/// @warning Funci�n interna: no llamar.
		void _SetSize(const Vector2ui size);

		/// @brief Establece el n�mero de canales (red green blue, etc...).
		/// @param numChannels N�mero de canales.
		/// @warning Funci�n interna: no llamar.
		void _SetNumberOfChannels(USize32 numChannels);

		/// @brief Establece la imagen GPU de la textura.
		/// @param image Imagen en la GPU.
		/// @warning Funci�n interna: no llamar.
		void _SetImage(OwnedPtr<GRAPHICS::GpuImage> image);

	private:

		/// @note En p�xeles.
		Vector2ui m_size;
		USize32 m_numChannels = 0;

		UniquePtr<GRAPHICS::GpuImage> m_image;

	};

}
