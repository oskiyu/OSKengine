#pragma once

#include "ToString.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Formatos que puede tener una textura / imagen.
	/// </summary>
	enum class Format {

		// COLOR
		RGBA8_UNORM,
		RGBA8_SRGB,
		RGBA16_SFLOAT,
		RGBA32_SFLOAT,
		BGRA8_SRGB,

		RG16_SFLOAT,
		RG32_SFLOAT,

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

	/// <summary>
	/// Devuelve el formato seg�n el string dado.
	/// </summary>
	/// <param name="formatStr">String, tal y como aparece en el enum Format.</param>
	/// <returns>Formato correspondiente.</returns>
	/// 
	/// @throws std::runtime_exception si no corresponde a ning�n formato.
	Format GetFormatFromString(const std::string& formatStr);

}

template <> std::string OSK::ToString<OSK::GRAPHICS::Format>(const OSK::GRAPHICS::Format& format);
