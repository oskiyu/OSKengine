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
	}
}
