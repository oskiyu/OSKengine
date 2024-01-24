#pragma once

#include "IAsset.h"
#include "Spline3D.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL PreBuiltSpline3D : public IAsset {

	public:


		explicit PreBuiltSpline3D(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::PreBuiltSpline3D");

		Spline3D& _Get();

		const Spline3D& Get() const;
		Spline3D GetCopy() const;

	private:

		Spline3D m_spline{};

	};

}
