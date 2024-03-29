#pragma once

#include "ToString.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Formatos que puede tener una textura / imagen.
	/// </summary>
	enum class Format {
		UNKNOWN = 0,

		// COLOR
		RGBA8_UNORM,
		RGBA8_SRGB,
		RGBA8_UINT,

		RGB10A2_UNORM,

		RGBA16_SFLOAT,

		RGBA32_SFLOAT,

		BGRA8_SRGB,

		R16_SFLOAT,

		RG16_SFLOAT,
		RG32_SFLOAT,

		// DEPTH-STENCIL
		D32S8_SFLOAT_SUINT,
		D24S8_SFLOAT_SUINT,

		// DEPTH
		D32_SFLOAT,
		D16_UNORM
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

	/// <summary>
	/// Devuelve el formato seg�n el string dado.
	/// </summary>
	/// <param name="formatStr">String, tal y como aparece en el enum Format.</param>
	/// <returns>Formato correspondiente.</returns>
	/// 
	/// @throws std::runtime_exception si no corresponde a ning�n formato.
	Format GetFormatFromString(const std::string& formatStr);

	bool FormatSupportsStencil(Format format);

}

template <> std::string OSK::ToString<OSK::GRAPHICS::Format>(const OSK::GRAPHICS::Format& format);
