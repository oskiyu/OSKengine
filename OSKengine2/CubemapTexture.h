#pragma once

#include "IAsset.h"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "IGpuImage.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL CubemapTexture : public IAsset {

	public:

		CubemapTexture(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::CubemapTexture");

		/// <summary>
		/// Imagen guardada en la GPU.
		/// Para renderizado.
		/// </summary>
		GRAPHICS::GpuImage* GetGpuImage() const;

		void _SetImage(OwnedPtr<GRAPHICS::GpuImage> image);

	private:

		UniquePtr<GRAPHICS::GpuImage> image;

	};

}
