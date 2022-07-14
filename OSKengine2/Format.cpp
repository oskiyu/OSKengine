#include "Format.h"
#include "FormatDx12.h"
#include "FormatVulkan.h"

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

	case Format::RGBA16_SFLOAT:
		return VK_FORMAT_R16G16B16A16_SFLOAT;

	case Format::RGB16_SFLOAT:
		return VK_FORMAT_R16G16B16_SFLOAT;

	case Format::RGBA32_SFLOAT:
		return VK_FORMAT_R32G32B32A32_SFLOAT;

	case Format::B8G8R8A8_SRGB:
		return VK_FORMAT_B8G8R8A8_UNORM;

	case Format::D32_SFLOAT:
		return VK_FORMAT_D32_SFLOAT;

	case Format::D32S8_SFLOAT_SUINT:
		return VK_FORMAT_D32_SFLOAT_S8_UINT;

	case Format::D24S8_SFLOAT_SUINT:
		return VK_FORMAT_D24_UNORM_S8_UINT;
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

		case Format::RGB16_SFLOAT:
			return 3 * 2;
		case Format::RGBA16_SFLOAT:
			return 4 * 2;

		case Format::RGBA32_SFLOAT:
			return 4 * 4;

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
	case OSK::GRAPHICS::Format::RGB16_SFLOAT:
		return "RGB16_SFLOAT";
	case OSK::GRAPHICS::Format::RGBA16_SFLOAT:
		return "RGBA16_SFLOAT";
	case OSK::GRAPHICS::Format::RGBA32_SFLOAT:
		return "RGBA32_SFLOAT";

	default:
		return "UNKNOWN";
	}
}

OSK::GRAPHICS::Format OSK::GRAPHICS::GetFormatFromString(const std::string& formatStr) {
	static Format formats[] = {
		Format::RGB8_UNORM,
		Format::RGBA8_UNORM,
		Format::RGB16_SFLOAT,
		Format::RGBA16_SFLOAT,
		Format::RGBA32_SFLOAT,
		Format::B8G8R8A8_SRGB,
		Format::D32S8_SFLOAT_SUINT,
		Format::D24S8_SFLOAT_SUINT,
		Format::D32_SFLOAT
	};

	for (TSize i = 0; i < _countof(formats); i++)
		if (ToString<Format>(formats[i]) == formatStr)
			return formats[i];

	OSK_ASSERT(false, "No se encuentra el formato " + formatStr + ".");

	return Format::RGBA8_UNORM;
}
