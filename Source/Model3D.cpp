#include "Model3D.h"

#include "GpuMesh3D.h"
#include "IGpuImage.h"

#include "InvalidArgumentException.h"

using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

Model3D::Model3D(const std::string& assetFile)
	: IAsset(assetFile) {

}

const GRAPHICS::GpuModel3D& Model3D::GetModel() const {
	return m_model3D;
}

GRAPHICS::GpuModel3D& Model3D::_GetModel() {
	return m_model3D;
}
