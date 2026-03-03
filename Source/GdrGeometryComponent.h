#pragma once

#include "Component.h"
#include "GpuGeometryDesc.h"

#include "UniquePtr.hpp"

namespace OSK::ECS {

	/// @brief Referencia una geometría de GPU
	/// de un render-graph.
	struct GdrGeometryComponent {

		OSK_COMPONENT("OSK::GdrGeometryComponent");

		GdrGeometryComponent(GRAPHICS::GpuGeometryDesc&& desc) 
			: gdrGeometry(MakeUnique<GRAPHICS::GpuGeometryDesc>(std::move(desc)))
		{}

		UniquePtr<GRAPHICS::GpuGeometryDesc> gdrGeometry;

	};

}
