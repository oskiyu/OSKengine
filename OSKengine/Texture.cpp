#include "Texture.h"

using namespace OSK;

VkFormat OSK::ToNative(TextureFormat format) {
	switch (format) {

	case TextureFormat::RGB_8bits:
		return VK_FORMAT_R8G8B8_SRGB;

	case TextureFormat::RGBA_8bits:
		return VK_FORMAT_R8G8B8A8_SRGB;

	case TextureFormat::INTERNAL_FONT:
		return VK_FORMAT_R8G8B8A8_SRGB;

	default:
		return VK_FORMAT_R8G8B8_SRGB;
	}
}

uint32_t OSK::GetNumberOfPixelsFromFormat(TextureFormat format) {
	switch (format) {

	case TextureFormat::RGB_8bits:
		return 3;

	case TextureFormat::RGBA_8bits:
		return 4;

	case TextureFormat::INTERNAL_FONT:
		return 4;

	default:
		return 3;
	}
}
