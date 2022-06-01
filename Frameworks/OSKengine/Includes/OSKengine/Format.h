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

	/// <summary>
	/// Devuelve el n�mero de bytes que ocupa un p�xel de un formato dado.
	/// </summary>
	/// <param name="format">Formato de imagen.</param>
	/// <returns>N�mero de bytes por pixel.</returns>
	unsigned int GetFormatNumberOfBytes(Format format);

	/// <summary>
	/// Devuelve un formato de color que tenga el n�mero de canales deseado.
	/// </summary>
	/// <returns>
	/// RGB8_UNORM o RGBA8_UNORM.
	/// Si se pasa un n�mero inv�lido, devolver� RGBA8_UNORM.</returns>
	Format GetColorFormat(unsigned int numChannels);

}
