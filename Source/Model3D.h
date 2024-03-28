#pragma once

#include "IAsset.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "IGpuImage.h"
#include "GpuBuffer.h"

#include "GpuModel3D.h"
#include "IBottomLevelAccelerationStructure.h"

#include "Animator.h"

#include <string>

#include "HashMap.hpp"
#include "VertexAttributes.h"

#include <any>

namespace OSK::GRAPHICS {
	class IMaterialSlot;
}

namespace OSK::ASSETS {

	/// @brief Un modelo 3D, para el renderizado 3D.
	/// Está compuesto por una serie de meshes 3D.
	class OSKAPI_CALL Model3D : public IAsset {

	public:

		explicit Model3D(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::Model3D");

		const GRAPHICS::GpuModel3D& GetModel() const;
		GRAPHICS::GpuModel3D& _GetModel();

	private:

		GRAPHICS::GpuModel3D m_model3D{};

	};

}
