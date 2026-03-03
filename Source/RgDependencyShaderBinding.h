#pragma once

#include <string>

namespace OSK::GRAPHICS {

	/// @brief Caso especial de una dependencia
	/// que se lee en un shader, ya sea de
	/// buffer o de imagen.
	struct RgDependencyShaderBinding {
		std::string slotName;
		std::string bindingName;
	};

}
