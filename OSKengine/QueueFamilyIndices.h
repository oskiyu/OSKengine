#pragma once

#include <optional>

namespace OSK::VULKAN {

	struct QueueFamilyIndices {

		std::optional<unsigned int> GraphicsFamily;

		std::optional<unsigned int> PresentFamily; //surface support

		bool IsComplete();

	};

}