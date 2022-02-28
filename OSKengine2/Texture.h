#pragma once

#include "IAsset.h"
#include "OwnedPtr.h"
#include "Vector2.hpp"

namespace OSK {

	class GpuImage;

	class OSKAPI_CALL Texture : public IAsset {

	public:

		Texture(const std::string& assetFile);
		~Texture();

		OSK_ASSET_TYPE_REG("Texture");

		Vector2ui GetSize() const;
		TSize GetNumberOfChannels() const;
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
