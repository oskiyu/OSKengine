#include "PreBuiltSpline3D.h"

using namespace OSK;
using namespace OSK::ASSETS;


PreBuiltSpline3D::PreBuiltSpline3D(const std::string& assetFile) : IAsset(assetFile) {

}

Spline3D& PreBuiltSpline3D::_Get() {
	return m_spline;
}

const Spline3D& PreBuiltSpline3D::Get() const {
	return m_spline;
}

Spline3D PreBuiltSpline3D::GetCopy() const {
	return m_spline;
}
