#pragma once

enum VkFormat;

namespace OSK {

	enum class Format;

	/// <summary>
	/// Obtiene la representaci�n nativa del formato para Vulkan.
	/// </summary>
	VkFormat GetFormatVulkan(Format format);

}