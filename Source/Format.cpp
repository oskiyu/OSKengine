#include "Format.h"
#include "FormatVk.h"

#include "Assert.h"

#include <vulkan/vulkan.h>
#include "NotImplementedException.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND
VkFormat OSK::GRAPHICS::GetFormatVk(Format format) {
	switch (format) {

	case Format::RGBA8_UNORM:
		return VK_FORMAT_R8G8B8A8_UNORM;

	case Format::RGBA8_SRGB:
		return VK_FORMAT_R8G8B8A8_SRGB;

	case Format::RGBA8_UINT:
		return VK_FORMAT_R8G8B8A8_UINT;

	case Format::RGB10A2_UNORM:
		return VK_FORMAT_A2R10G10B10_UNORM_PACK32;

	case Format::RGBA16_SFLOAT:
		return VK_FORMAT_R16G16B16A16_SFLOAT;

	case Format::RGBA32_SFLOAT:
		return VK_FORMAT_R32G32B32A32_SFLOAT;

	case Format::BGRA8_SRGB:
		return VK_FORMAT_B8G8R8A8_UNORM;

	case Format::RG16_SFLOAT:
		return VK_FORMAT_R16G16_SFLOAT;

	case Format::RG32_SFLOAT:
		return VK_FORMAT_R32G32_SFLOAT;

	case Format::D32_SFLOAT:
		return VK_FORMAT_D32_SFLOAT;

	case Format::D16_UNORM:
		return VK_FORMAT_D16_UNORM;

	case Format::R16_SFLOAT:
		return VK_FORMAT_R16_SFLOAT;

	case Format::D32S8_SFLOAT_SUINT:
		return VK_FORMAT_D32_SFLOAT_S8_UINT;

	case Format::D24S8_SFLOAT_SUINT:
		return VK_FORMAT_D24_UNORM_S8_UINT;

	case Format::UNKNOWN:
		return VK_FORMAT_UNDEFINED;

	}

	OSK_ASSERT(false, NotImplementedException());
	return VK_FORMAT_UNDEFINED;
}

OSK::GRAPHICS::Format OSK::GRAPHICS::FormatVkToEngine(VkFormat format) {
	switch (format) {

	case VK_FORMAT_R8G8B8A8_UNORM:
		return Format::RGBA8_UNORM;

	case VK_FORMAT_R8G8B8A8_SRGB:
		return Format::RGBA8_SRGB;

	case VK_FORMAT_R8G8B8A8_UINT:
		return Format::RGBA8_UINT;

	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		return Format::RGB10A2_UNORM;

	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return Format::RGBA16_SFLOAT;

	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return Format::RGBA32_SFLOAT;

	case VK_FORMAT_B8G8R8A8_UNORM:
		return Format::BGRA8_SRGB;

	case VK_FORMAT_R16G16_SFLOAT:
		return Format::RG16_SFLOAT;

	case VK_FORMAT_R32G32_SFLOAT:
		return Format::RG32_SFLOAT;

	case VK_FORMAT_D32_SFLOAT:
		return Format::D32_SFLOAT;

	case VK_FORMAT_D16_UNORM:
		return Format::D16_UNORM;

	case VK_FORMAT_R16_SFLOAT:
		return Format::R16_SFLOAT;

	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		return Format::D32S8_SFLOAT_SUINT;

	case VK_FORMAT_D24_UNORM_S8_UINT:
		return Format::D24S8_SFLOAT_SUINT;

	case VK_FORMAT_UNDEFINED:
		return Format::UNKNOWN;

	}

	OSK_ASSERT(false, NotImplementedException());
	return Format::UNKNOWN;
}
#endif // OSK_USE_VULKAN_BACKEND

unsigned int OSK::GRAPHICS::GetFormatNumberOfBytes(Format format) {
	switch (format) {

		case Format::RGBA8_UNORM:
		case Format::RGBA8_UINT:
		case Format::RGBA8_SRGB:
		case Format::BGRA8_SRGB:
		case Format::RGB10A2_UNORM:
			return 8 * 4 / 8;

		case Format::RGBA16_SFLOAT:
			return 16 * 4 / 8;

		case Format::RG16_SFLOAT:
			return 16 * 2 / 8;

		case Format::RG32_SFLOAT:
			return 32 * 2 / 8;

		case Format::RGBA32_SFLOAT:
			return 32 * 4 / 8;

		case Format::D32_SFLOAT:
		case Format::D24S8_SFLOAT_SUINT:
			return 32 / 8;

		case Format::D16_UNORM:
		case Format::R16_SFLOAT:
			return 16 / 8;

		case Format::D32S8_SFLOAT_SUINT:
			return (32 + 8) / 8;

	}

	OSK_ASSERT(false, NotImplementedException());
	return 0;
}

OSK::GRAPHICS::Format OSK::GRAPHICS::GetColorFormat(unsigned int numberOfChannels) {
	return OSK::GRAPHICS::Format::RGBA8_UNORM;
}

template <> std::string OSK::ToString<OSK::GRAPHICS::Format>(const OSK::GRAPHICS::Format& format) {
	switch (format) {
	case OSK::GRAPHICS::Format::BGRA8_SRGB:
		return "B8G8R8A8_SRGB";
	case OSK::GRAPHICS::Format::D24S8_SFLOAT_SUINT:
		return "D24S8_SFLOAT_SUINT";
	case OSK::GRAPHICS::Format::D32S8_SFLOAT_SUINT:
		return "D32S8_SFLOAT_SUINT";
	case OSK::GRAPHICS::Format::D32_SFLOAT:
		return "D32_SFLOAT";
	case OSK::GRAPHICS::Format::D16_UNORM:
		return "D16_UNORM";
	case OSK::GRAPHICS::Format::R16_SFLOAT:
		return "R16_SFLOAT";
	case OSK::GRAPHICS::Format::RGBA8_UNORM:
		return "RGBA8_UNORM";
	case OSK::GRAPHICS::Format::RGBA8_SRGB:
		return "RGBA8_SRGB";
	case OSK::GRAPHICS::Format::RGBA8_UINT:
		return "RGBA8_UINT";
	case OSK::GRAPHICS::Format::RGB10A2_UNORM:
		return "RGB10A2_UNORM";
	case OSK::GRAPHICS::Format::RGBA16_SFLOAT:
		return "RGBA16_SFLOAT";
	case OSK::GRAPHICS::Format::RGBA32_SFLOAT:
		return "RGBA32_SFLOAT";
	case OSK::GRAPHICS::Format::RG16_SFLOAT:
		return "RG16_SFLOAT";
	case OSK::GRAPHICS::Format::RG32_SFLOAT:
		return "RG32_SFLOAT";

	default:
		return "UNKNOWN";
	}
}

OSK::GRAPHICS::Format OSK::GRAPHICS::GetFormatFromString(const std::string& formatStr) {
	static Format formats[] = {
		Format::RGBA8_UNORM,
		Format::RGBA16_SFLOAT,
		Format::RGBA32_SFLOAT,
		Format::BGRA8_SRGB,
		Format::D32S8_SFLOAT_SUINT,
		Format::D24S8_SFLOAT_SUINT,
		Format::D32_SFLOAT,
		Format::D16_UNORM,
		Format::R16_SFLOAT,
		Format::RG16_SFLOAT,
		Format::RG32_SFLOAT,
	};

	for (const auto format : formats)
		if (ToString<Format>(format) == formatStr)
			return format;

	OSK_ASSERT(false, NotImplementedException());

	return Format::RGBA8_UNORM;
}

bool OSK::GRAPHICS::FormatSupportsStencil(Format format) {
	return
		format == Format::D32S8_SFLOAT_SUINT ||
		format == Format::D24S8_SFLOAT_SUINT;
}
