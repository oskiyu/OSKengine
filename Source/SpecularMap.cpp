// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "SpecularMap.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

SpecularMap::SpecularMap(const std::string& assetFile) 
	: IAsset (assetFile) {

}

GpuImage* SpecularMap::GetCubemapImage() {
	return iblCubemap.GetPointer();
}

void SpecularMap::_SetCubemapImage(UniquePtr<GRAPHICS::GpuImage>&& image) {
	iblCubemap = std::move(image);
}

GpuImage* SpecularMap::GetLookUpTable() {
	return lookUpTable.GetPointer();
}

void SpecularMap::_SetLookUpTable(UniquePtr<GRAPHICS::GpuImage>&& image) {
	lookUpTable = std::move(image);
}
