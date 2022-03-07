#pragma once

enum VkFormat;

namespace OSK::GRAPHICS {
	
	enum class Format;

	/// <summary>
	/// Obtiene la representación nativa del formato para Vulkan.
	/// </summary>
	VkFormat GetFormatVulkan(Format format);

}