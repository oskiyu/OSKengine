#pragma once

#include "Component.h"
#include "Color.hpp"

namespace OSK::ECS {

	/// @brief Parámetros de renderizado PBR.
	struct GdrMaterialComponent {

		OSK_COMPONENT("OSK::GdrMaterialComponent");

		float metallic = 0.0f;
		float roughness = 0.0f;
		Color emmisive = Color::Empty;

	};

}
