#pragma once

#include "Color.hpp"
#include "Vector2.hpp"

namespace OSK::GRAPHICS {

	struct PbrMaterialInfo {

		alignas(16) Color emissiveColor = Color::Black;
		alignas(16) Vector2f roughnessMetallic = Vector2f::Zero;
		alignas(16) int hasNormalTexture = 0;

	};

}
