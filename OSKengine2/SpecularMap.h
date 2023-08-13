// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "IAsset.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL SpecularMap : public IAsset {

	public:

		SpecularMap(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::SpecularMap");

		GRAPHICS::GpuImage* GetCubemapImage();
		const GRAPHICS::GpuImage* GetCubemapImage() const { return iblCubemap.GetPointer(); }
		void _SetCubemapImage(OwnedPtr<GRAPHICS::GpuImage> image);

		GRAPHICS::GpuImage* GetLookUpTable();
		const GRAPHICS::GpuImage* GetLookUpTable() const { return lookUpTable.GetPointer(); }
		void _SetLookUpTable(OwnedPtr<GRAPHICS::GpuImage> lut);

	private:

		UniquePtr<GRAPHICS::GpuImage> iblCubemap;
		UniquePtr<GRAPHICS::GpuImage> lookUpTable;

	};

}
