#include "GpuImageCreateInfo.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageCreateInfo GpuImageCreateInfo::CreateDefault1D(uint32_t resolution, Format format, GpuImageUsage usage) {
	GpuImageCreateInfo output{};
	output.resolution = Vector3ui{ resolution, 1, 1 };
	output.format = format;
	output.usage = usage;
	output.dimension = GpuImageDimension::d1D;

	return output;
}

GpuImageCreateInfo GpuImageCreateInfo::CreateDefault2D(const Vector2ui& resolution, Format format, GpuImageUsage usage) {
	GpuImageCreateInfo output{};
	output.resolution = Vector3ui{ resolution.x, resolution.y, 1 };
	output.format = format;
	output.usage = usage;
	output.dimension = GpuImageDimension::d2D;

	return output;
}

GpuImageCreateInfo GpuImageCreateInfo::CreateDefault3D(const Vector3ui& resolution, Format format, GpuImageUsage usage) {
	GpuImageCreateInfo output{};
	output.resolution = resolution;
	output.format = format;
	output.usage = usage;
	output.dimension = GpuImageDimension::d3D;

	return output;
}
