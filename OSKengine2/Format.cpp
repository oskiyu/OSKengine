#include "Format.h"
#include "FormatDx12.h"
#include "FormatVulkan.h"

#include <dxgi1_6.h>
#include <vulkan/vulkan.h>

DXGI_FORMAT OSK::GetFormatDx12(Format format) {
	switch (format) {
		case Format::RGBA8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}

VkFormat OSK::GetFormatVulkan(Format format) {
	switch (format) {
	case Format::RGBA8_UNORM:
		return VK_FORMAT_R8G8B8A8_UNORM;

	case Format::B8G8R8A8_SRGB:
		return VK_FORMAT_B8G8R8A8_SRGB;

	case Format::D32_SFLOAT:
		return VK_FORMAT_D32_SFLOAT;

	case Format::D32S8_SFLOAT_SUINT:
		return VK_FORMAT_D32_SFLOAT_S8_UINT;
	}
}

unsigned int OSK::GetFormatNumberOfBytes(Format format) {
	switch (format) {

		case Format::RGBA8_UNORM:
		case Format::B8G8R8A8_SRGB:
			return 3;

		case Format::D32_SFLOAT:
			return 4;

		case Format::D32S8_SFLOAT_SUINT:
			return 5;

	}

	return 0;
}
