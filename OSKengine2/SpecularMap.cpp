// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "SpecularMap.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

SpecularMap::SpecularMap(const std::string& assetFile) 
	: IAsset (assetFile) {

}

GpuImage* SpecularMap::GetCubemapImage() const {
	return iblCubemap.GetPointer();
}

void SpecularMap::_SetCubemapImage(OwnedPtr<GpuImage> image) {
	iblCubemap = image.GetPointer();
}

GpuImage* SpecularMap::GetLookUpTable() const {
	return lookUpTable.GetPointer();
}

void SpecularMap::_SetLookUpTable(OwnedPtr<GpuImage> image) {
	lookUpTable = image.GetPointer();
}
