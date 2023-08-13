#include "IrradianceMap.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

IrradianceMap::IrradianceMap(const std::string& assetFile)
	: IAsset(assetFile) {

}

GRAPHICS::GpuImage* IrradianceMap::GetGpuImage() {
	return gpuImage.GetPointer();
}

void IrradianceMap::_SetGpuImage(OwnedPtr<GRAPHICS::GpuImage> image) {
	gpuImage = image.GetPointer();
}

void IrradianceMap::_SetOriginalCubemap(OwnedPtr<GRAPHICS::GpuImage> image) {
	originalCubemap = image.GetPointer();
}

GpuImage* IrradianceMap::GetOriginalCubemap() {
	return originalCubemap.GetPointer();
}
