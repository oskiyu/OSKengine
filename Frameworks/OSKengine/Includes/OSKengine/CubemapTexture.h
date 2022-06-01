#pragma once

#include "IAsset.h"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "IGpuImage.h"

namespace OSK::ASSETS {

	/// <summary>
	/// Representa una textura para un cubemap.
	/// Se puede usar para renderizar un skybox.
	/// </summary>
	/// 
	/// @note Representa un array de imágenes con 6 elementos.
	/// 
	/// @warning Únicamente para usarse como lectura, no se puede
	/// renderizar sobre un cubemap representado por esta clase.
	class OSKAPI_CALL CubemapTexture : public IAsset {

	public:

		CubemapTexture(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::CubemapTexture");

		/// <summary>
		/// Imagen guardada en la GPU.
		/// Para renderizado.
		/// </summary>
		GRAPHICS::GpuImage* GetGpuImage() const;

		/// <summary>
		/// Establece la imagen que contiene el cubemap.
		/// </summary>
		/// 
		/// @warning Función interna: no llamar.
		void _SetImage(OwnedPtr<GRAPHICS::GpuImage> image);

	private:

		UniquePtr<GRAPHICS::GpuImage> image;

	};

}
