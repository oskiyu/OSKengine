#pragma once

#include "HashMap.hpp"
#include <unordered_set>
#include <string>

namespace OSK::GRAPHICS {

	/// @brief Dependencias de un render-pass
	/// respecto a otros render-passes.
	struct RenderPassDependencies {

		std::unordered_set<std::string, StringHasher, std::equal_to<>> executeAfterThese{};

		static RenderPassDependencies Empty() {
			return RenderPassDependencies{};
		};

	};

}
