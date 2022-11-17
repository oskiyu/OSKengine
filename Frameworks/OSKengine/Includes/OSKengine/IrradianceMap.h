#pragma once

#include "IAsset.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"

namespace OSK::ASSETS {

	/// <summary>
	/// Un irradiance map es un cubemap que contiene información
	/// sobre el color de la luz procedente de la escena.
	/// 
	/// Se puede usar para darle un color más realista a los
	/// modelos 3D de una escena.
	/// </summary>
	class OSKAPI_CALL IrradianceMap : public IAsset {

	public:

		IrradianceMap(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::IrradianceMap");

		/// <summary> Devuelve la imagen con el irradiance map. </summary>
		/// 
		/// @note Es un cubemap.
		GRAPHICS::GpuImage* GetGpuImage() const;

		/// <summary>
		/// Devuelve el cubemap a partir del que se 
		/// creó el irradiance map.
		/// </summary>
		GRAPHICS::GpuImage* GetOriginalCubemap() const;


		/// <summary> Establece la imagen que contiene el irradiance map. </summary>
		/// <param name="image">Irradiance map generado.</param>
		void _SetGpuImage(OwnedPtr<GRAPHICS::GpuImage> image);

		/// <summary> 
		/// Establece la imagen que contiene el cubemap original sobre 
		/// el que se genera el irradiance map. 
		/// </summary>
		/// <param name="image">Irradiance map generado.</param>
		void _SetOriginalCubemap(OwnedPtr<GRAPHICS::GpuImage> image);

	private:

		/// <summary> Irradiance map ya generado. </summary>
		UniquePtr<GRAPHICS::GpuImage> gpuImage;
		/// <summary> Cubemap original sobre el que se genera el irradiance map. </summary>
		UniquePtr<GRAPHICS::GpuImage> originalCubemap;

	};

}
