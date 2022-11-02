#pragma once

#include "OSKmacros.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Tipo de filtro que se aplicar� al acceder a la imagen.
	/// </summary>
	enum class GpuImageFilteringType {

		/// <summary>
		/// Valor linealmente interpolado con los colindantes.
		/// </summary>
		LIENAR,

		/// <summary>
		/// El valor no se interpola.
		/// </summary>
		NEAREST,

		/// <summary>
		/// El valor se interpola c�bicamente con los colindantes.
		/// </summary>
		CUBIC
	};

	/// <summary>
	/// Describe qu� color se seleccionar� al acceder fuera de las
	/// coordenadas de la textura.
	/// </summary>
	enum class GpuImageAddressMode {

		/// <summary>
		/// La textura se repite infinitamente.
		/// </summary>
		REPEAT,

		/// <summary>
		/// La textura se repite infinitamente, d�ndose la vuelta.
		/// </summary>
		MIRRORED_REPEAT,

		/// <summary>
		/// Se devuelve el color del borde m�s cercano.
		/// </summary>
		EDGE,

		/// <summary>
		/// Borde negro.
		/// </summary>
		BACKGROUND_BLACK,

		/// <summary>
		/// Borde blanco.
		/// </summary>
		BACKGROUND_WHITE,

	};


	/// <summary>
	/// Configuraci�n del nivel de mipmaps m�nimo y m�ximo para im�genes
	/// en la GPU.
	/// </summary>
	enum class GpuImageMipmapMode {

		/// <summary>
		/// No se usar�n mipmaps.
		/// </summary>
		NONE,

		/// <summary>
		/// Uso de mipmaps por defecto.
		/// 
		/// El nivel m�s bajo ser� 0 (imagen original),
		/// y el nivel m�s alto ser� el mayor posible, dependiendo
		/// del tama�o de la imagen.
		/// </summary>
		AUTO,

		/// <summary>
		/// Se usar�n los niveles m�nimo y m�ximo especificados
		/// en GpuImageSamplerDesc::minMipLevel y GpuImageSamplerDesc::maxMipLevel.
		/// </summary>
		CUSTOM

	};


	/// <summary>
	/// Establece propiedades b�sicas sobre el
	/// acceso a la imagen.
	/// </summary>
	struct GpuImageSamplerDesc {
		GpuImageFilteringType filteringType = GpuImageFilteringType::LIENAR;
		GpuImageAddressMode addressMode = GpuImageAddressMode::REPEAT;

		GpuImageMipmapMode mipMapMode = GpuImageMipmapMode::AUTO;


		/// <summary>
		/// Divel de mipmap m�s bajo.
		/// </summary>
		/// 
		/// @note El valor 0 identifica a la imagen original.
		/// @warning �nicamente se usa este valor si mipMapMode == GpuImageMipmapMode::CUSTOM.
		TSize minMipLevel = 0;

		/// <summary>
		/// Divel de mipmap m�s alto.
		/// </summary>
		/// 
		/// @note El valor 0 identifica a la imagen original.
		/// @warning �nicamente se usa este valor si mipMapMode == GpuImageMipmapMode::CUSTOM.
		TSize maxMipLevel = 0;

		static GpuImageSamplerDesc CreateDefault() {
			return GpuImageSamplerDesc{ .mipMapMode = GpuImageMipmapMode::NONE };
		}

		static GpuImageSamplerDesc CreateTextureDefault() {
			return GpuImageSamplerDesc{ .mipMapMode = GpuImageMipmapMode::AUTO };
		}

	};

}
