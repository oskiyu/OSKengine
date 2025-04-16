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

void IrradianceMap::_SetGpuImage(UniquePtr<GRAPHICS::GpuImage>&& image) {
	gpuImage = std::move(image);
}

void IrradianceMap::_SetOriginalCubemap(UniquePtr<GRAPHICS::GpuImage>&& image) {
	originalCubemap = std::move(image);
}

GpuImage* IrradianceMap::GetOriginalCubemap() {
	return originalCubemap.GetPointer();
}
