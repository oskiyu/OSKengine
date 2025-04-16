#include "CubemapTexture.h"

using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

CubemapTexture::CubemapTexture(const std::string& assetFile) 
	: IAsset(assetFile)  {

}

const GpuImage* CubemapTexture::GetGpuImage() const {
	return image.GetPointer();
}

void CubemapTexture::_SetImage(UniquePtr<GRAPHICS::GpuImage>&& image) {
	this->image = std::move(image);
}
