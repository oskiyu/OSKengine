#include "GpuMemoryTypes.h"
#include "OSKmacros.h"

template <> inline std::string OSK::ToString<OSK::GRAPHICS::GpuSharedMemoryType>(const OSK::GRAPHICS::GpuSharedMemoryType& type) {
	switch (type) {
	case OSK::GRAPHICS::GpuSharedMemoryType::GPU_AND_CPU:
		return "GpuSharedMemoryType::GPU_AND_CPU";
	case OSK::GRAPHICS::GpuSharedMemoryType::GPU_ONLY:
		return "GpuSharedMemoryType::GPU_ONLY";
	}

	return "Unknown";
}

template <> inline std::string OSK::ToString<OSK::GRAPHICS::GpuMemoryUsage>(const OSK::GRAPHICS::GpuMemoryUsage& usage) {
	switch (usage) {
	case OSK::GRAPHICS::GpuMemoryUsage::BUFFER:
		return "GpuMemoryUsage::BUFFER";
	case OSK::GRAPHICS::GpuMemoryUsage::IMAGE:
		return "GpuMemoryUsage::IMAGE";
	}

	return "Unknown";
}

template <> std::string OSK::ToString<OSK::GRAPHICS::GpuBufferUsage>(const OSK::GRAPHICS::GpuBufferUsage& usage) {
	TSize numFlags = 0;
	
	std::string output = "";

	if (EFTraits::HasFlag(usage, OSK::GRAPHICS::GpuBufferUsage::UNIFORM_BUFFER)) {
		if (numFlags != 0)
			output += " | ";
		output += "GpuBufferUsage::UNIFORM_BUFFER";

		numFlags++;
	}

	if (EFTraits::HasFlag(usage, OSK::GRAPHICS::GpuBufferUsage::VERTEX_BUFFER)) {
		if (numFlags != 0)
			output += " | ";
		output += "GpuBufferUsage::VERTEX_BUFFER";

		numFlags++;
	}

	if (EFTraits::HasFlag(usage, OSK::GRAPHICS::GpuBufferUsage::INDEX_BUFFER)) {
		if (numFlags != 0)
			output += " | ";
		output += "GpuBufferUsage::INDEX_BUFFER";

		numFlags++;
	}

	if (EFTraits::HasFlag(usage, OSK::GRAPHICS::GpuBufferUsage::TRANSFER_SOURCE)) {
		if (numFlags != 0)
			output += " | ";
		output += "GpuBufferUsage::TRANSFER_SOURCE";

		numFlags++;
	}

	if (EFTraits::HasFlag(usage, OSK::GRAPHICS::GpuBufferUsage::TRANSFER_DESTINATION)) {
		if (numFlags != 0)
			output += " | ";
		output += "GpuBufferUsage::TRANSFER_DESTINATION";

		numFlags++;
	}

	if (EFTraits::HasFlag(usage, OSK::GRAPHICS::GpuBufferUsage::RT_ACCELERATION_STRUCTURE)) {
		if (numFlags != 0)
			output += " | ";
		output += "GpuBufferUsage::RT_ACCELERATION_STRUCTURE";

		numFlags++;
	}

	if (EFTraits::HasFlag(usage, OSK::GRAPHICS::GpuBufferUsage::RT_SHADER_BINDING_TABLE)) {
		if (numFlags != 0)
			output += " | ";
		output += "GpuBufferUsage::RT_SHADER_BINDING_TABLE";

		numFlags++;
	}

	if (numFlags == 0)
		return "Unknown";

	return output;
}
