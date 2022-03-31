#pragma once

namespace OSK::GRAPHICS {

	/// <summary>
	/// Formatos que puede tener una textura / imagen.
	/// </summary>
	enum class Format {

		// COLOR
		RGB8_UNORM,
		RGBA8_UNORM,
		B8G8R8A8_SRGB,

		// DEPTH-STENCIL
		D32S8_SFLOAT_SUINT,
		D24S8_SFLOAT_SUINT,

		// DEPTH
		D32_SFLOAT

	};

	unsigned int GetFormatNumberOfBytes(Format format);

	Format GetColorFormat(unsigned int numChannels);

}
