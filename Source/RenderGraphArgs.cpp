#include "RenderGraphArgs.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

RgRelativeSizeImageRegisterArgs RgRelativeSizeImageRegisterArgs::From1D(float resolution) {
	RgRelativeSizeImageRegisterArgs output{};

	output.relativeResolution.x = resolution;
	output.relativeResolution.y = 1.0f;
	output.relativeResolution.z = 1.0f;

	return output;
}

RgRelativeSizeImageRegisterArgs RgRelativeSizeImageRegisterArgs::From2D(const Vector2f& resolution) {
	RgRelativeSizeImageRegisterArgs output{};

	output.relativeResolution.x = resolution.x;
	output.relativeResolution.y = resolution.y;
	output.relativeResolution.z = 1.0f;

	return output;
}


RgAbsoluteSizeImageRegisterArgs RgAbsoluteSizeImageRegisterArgs::From1D(const Vector2ui& resolution) {
	RgAbsoluteSizeImageRegisterArgs output{};

	output.resolution.x = resolution.x;
	output.resolution.y = resolution.y;
	output.resolution.z = 1;

	return output;
}

RgAbsoluteSizeImageRegisterArgs RgAbsoluteSizeImageRegisterArgs::From2D(const Vector2ui& resolution) {
	RgAbsoluteSizeImageRegisterArgs output{};

	output.resolution.x = resolution.x;
	output.resolution.y = resolution.y;
	output.resolution.z = 1;

	return output;
}
