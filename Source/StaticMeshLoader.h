#pragma once

#include "IGltfLoader.h"
#include "CpuModel3D.h"
#include "GpuModel3D.h"
#include "Vertex3D.h"

namespace OSK::ASSETS {

	/// <summary>
	/// Clase auxiliar que permite la carga de modelos 3D estáticos.
	/// </summary>
	class StaticMeshLoader {

	public:

		void Load(const CpuModel3D& model, GRAPHICS::GpuModel3D* output);

	};

}
