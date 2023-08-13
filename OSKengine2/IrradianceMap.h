#pragma once

#include "IAsset.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"

namespace OSK::ASSETS {
		
	/// @brief Un irradiance map es un cubemap que contiene información
	/// sobre el color de la luz procedente de la escena.
	/// 
	/// Se puede usar para darle un color más realista a los
	/// modelos 3D de una escena.
	class OSKAPI_CALL IrradianceMap : public IAsset {

	public:

		IrradianceMap(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::IrradianceMap");

		/// @brief Devuelve la imagen con el irradiance map.
		/// @return Es un cubemap.
		GRAPHICS::GpuImage* GetGpuImage();
		const GRAPHICS::GpuImage* GetGpuImage() const { return gpuImage.GetPointer(); }

		/// @return Devuelve el cubemap a partir del que se 
		/// creó el irradiance map.
		GRAPHICS::GpuImage* GetOriginalCubemap();
		const GRAPHICS::GpuImage* GetOriginalCubemap() const { return originalCubemap.GetPointer(); }


		/// @brief Establece la imagen que contiene el irradiance map.
		/// @param image Irradiance map generado.
		void _SetGpuImage(OwnedPtr<GRAPHICS::GpuImage> image);

		/// @brief Establece la imagen que contiene el cubemap original sobre 
		/// el que se genera el irradiance map. 
		/// @param image Cubemap original.
		void _SetOriginalCubemap(OwnedPtr<GRAPHICS::GpuImage> image);

	private:

		/// @brief Irradiance map ya generado.
		UniquePtr<GRAPHICS::GpuImage> gpuImage;
		/// @brief Irradiance map ya generado.
		UniquePtr<GRAPHICS::GpuImage> originalCubemap;

	};

}
