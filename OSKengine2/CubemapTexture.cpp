#include "CubemapTexture.h"

using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

CubemapTexture::CubemapTexture(const std::string& assetFile) 
	: IAsset(assetFile)  {

}

GpuImage* CubemapTexture::GetGpuImage() const {
	return image.GetPointer();
}

void CubemapTexture::_SetImage(OwnedPtr<GpuImage> image) {
	this->image = image.GetPointer();
}
