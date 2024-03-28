#pragma once

#include "IGltfLoader.h"
#include "Vertex3D.h"
#include "Bone.h"
#include "Animator.h"


namespace OSK::ASSETS {

	class OSKAPI_CALL AnimMeshLoader {

	public:

		void Load(const CpuModel3D& model, GRAPHICS::GpuModel3D* output);

	};

}
