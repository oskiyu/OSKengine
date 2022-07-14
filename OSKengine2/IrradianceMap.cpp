#include "IrradianceMap.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IrradianceMap::IrradianceMap(const std::string& assetFile)
	: IAsset(assetFile) {

}

GRAPHICS::GpuImage* IrradianceMap::GetGpuImage() const {
	return gpuImage.GetPointer();
}

void IrradianceMap::_SetGpuImage(OwnedPtr<GRAPHICS::GpuImage> image) {
	gpuImage = image.GetPointer();
}

void IrradianceMap::_SetOriginalCubemap(OwnedPtr<GRAPHICS::GpuImage> image) {
	originalCubemap = image.GetPointer();
}

GpuImage* IrradianceMap::GetOriginalCubemap() const {
	return originalCubemap.GetPointer();
}
