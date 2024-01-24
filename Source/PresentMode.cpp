#include "PresentMode.h"

template <> std::string OSK::ToString<OSK::GRAPHICS::PresentMode>(const OSK::GRAPHICS::PresentMode& mode) {
	switch (mode) {
		case OSK::GRAPHICS::PresentMode::VSYNC_OFF: return "PresentMode::VSYNC_OFF";
		case OSK::GRAPHICS::PresentMode::VSYNC_ON: return "PresentMode::VSYNC_ON";
		case OSK::GRAPHICS::PresentMode::VSYNC_ON_TRIPLE_BUFFER: return "PresentMode::VSYNC_ON_TRIPLE_BUFFER";
	}

	return "Unkwnown";
}
