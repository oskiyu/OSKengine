#include "Format.h"
#include "FormatDx12.h"
#include "FormatVulkan.h"
#include "FormatOgl.h"

#include "Assert.h"

#include <dxgi1_6.h>
#include <vulkan/vulkan.h>
#include <glad/glad.h>

DXGI_FORMAT OSK::GRAPHICS::GetFormatDx12(Format format) {
	switch (format) {
		case Format::RGB8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case Format::RGBA8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Format::B8G8R8A8_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

		case Format::D32_SFLOAT:
			return DXGI_FORMAT_D32_FLOAT;

		case Format::D24S8_SFLOAT_SUINT:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;

		case Format::D32S8_SFLOAT_SUINT:
			return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	}

	OSK_ASSERT(false, "El formato " + ToString<Format>(format) + " no está registrado en OSK::GRAPHICS::GetFormatDx12.");
	return DXGI_FORMAT_UNKNOWN;
}

VkFormat OSK::GRAPHICS::GetFormatVulkan(Format format) {
	switch (format) {
	case Format::RGB8_UNORM:
		return VK_FORMAT_R8G8B8_UNORM;

	case Format::RGBA8_UNORM:
		return VK_FORMAT_R8G8B8A8_UNORM;

	case Format::B8G8R8A8_SRGB:
		return VK_FORMAT_B8G8R8A8_UNORM;

	case Format::D32_SFLOAT:
		return VK_FORMAT_D32_SFLOAT;

	case Format::D32S8_SFLOAT_SUINT:
		return VK_FORMAT_D32_SFLOAT_S8_UINT;
	}

	OSK_ASSERT(false, "El formato " + ToString<Format>(format) + " no está registrado en OSK::GRAPHICS::GetFormatVulkan.");
	return VK_FORMAT_UNDEFINED;
}

unsigned int OSK::GRAPHICS::GetFormatNumberOfBytes(Format format) {
	switch (format) {

		case Format::RGB8_UNORM:
			return 4;

		case Format::RGBA8_UNORM:
		case Format::B8G8R8A8_SRGB:
			return 4;

		case Format::D32_SFLOAT:
		case Format::D24S8_SFLOAT_SUINT:
			return 4;

		case Format::D32S8_SFLOAT_SUINT:
			return 5;

	}

	OSK_ASSERT(false, "El formato " + ToString<Format>(format) + " no está registrado en OSK::GRAPHICS::GetFormatNumberOfBytes.");
	return 0;
}

OSK::GRAPHICS::Format OSK::GRAPHICS::GetColorFormat(unsigned int numberOfChannels) {
	if (numberOfChannels == 3)
		return OSK::GRAPHICS::Format::RGB8_UNORM;
	else
		return OSK::GRAPHICS::Format::RGBA8_UNORM;
}

unsigned int OSK::GRAPHICS::GetFormatOgl(GRAPHICS::Format format) {
	switch (format) {
	case GRAPHICS::Format::RGB8_UNORM:
		return GL_RGB8_SNORM;

	case GRAPHICS::Format::RGBA8_UNORM:
		return GL_RGBA8_SNORM;

	case GRAPHICS::Format::B8G8R8A8_SRGB:
		return GL_BGRA;

	case GRAPHICS::Format::D32_SFLOAT:
		return GL_R32F;

	case GRAPHICS::Format::D32S8_SFLOAT_SUINT:
		return GL_DEPTH32F_STENCIL8;

	case GRAPHICS::Format::D24S8_SFLOAT_SUINT:
		return GL_DEPTH24_STENCIL8;
	}

	OSK_ASSERT(false, "El formato " + ToString<Format>(format) + " no está registrado en OSK::GRAPHICS::GetFormatOgl.");
	return 0;
}

unsigned int OSK::GRAPHICS::GetFormatInternalOgl(Format format) {
	return GetFormatOgl(format);
}

template <> std::string OSK::ToString<OSK::GRAPHICS::Format>(const OSK::GRAPHICS::Format& format) {
	switch (format) {
	case OSK::GRAPHICS::Format::B8G8R8A8_SRGB:
		return "B8G8R8A8_SRGB";
	case OSK::GRAPHICS::Format::D24S8_SFLOAT_SUINT:
		return "D24S8_SFLOAT_SUINT";
	case OSK::GRAPHICS::Format::D32S8_SFLOAT_SUINT:
		return "D32S8_SFLOAT_SUINT";
	case OSK::GRAPHICS::Format::D32_SFLOAT:
		return "D32_SFLOAT";
	case OSK::GRAPHICS::Format::RGB8_UNORM:
		return "RGB8_UNORM";
	case OSK::GRAPHICS::Format::RGBA8_UNORM:
		return "RGBA8_UNORM";
	default:
		return "UNKNOWN";
		break;
	}
}