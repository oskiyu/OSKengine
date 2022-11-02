#pragma once

#include "OSKmacros.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Tipo de filtro que se aplicará al acceder a la imagen.
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
		/// El valor se interpola cúbicamente con los colindantes.
		/// </summary>
		CUBIC
	};

	/// <summary>
	/// Describe qué color se seleccionará al acceder fuera de las
	/// coordenadas de la textura.
	/// </summary>
	enum class GpuImageAddressMode {

		/// <summary>
		/// La textura se repite infinitamente.
		/// </summary>
		REPEAT,

		/// <summary>
		/// La textura se repite infinitamente, dándose la vuelta.
		/// </summary>
		MIRRORED_REPEAT,

		/// <summary>
		/// Se devuelve el color del borde más cercano.
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
	/// Configuración del nivel de mipmaps mínimo y máximo para imágenes
	/// en la GPU.
	/// </summary>
	enum class GpuImageMipmapMode {

		/// <summary>
		/// No se usarán mipmaps.
		/// </summary>
		NONE,

		/// <summary>
		/// Uso de mipmaps por defecto.
		/// 
		/// El nivel más bajo será 0 (imagen original),
		/// y el nivel más alto será el mayor posible, dependiendo
		/// del tamaño de la imagen.
		/// </summary>
		AUTO,

		/// <summary>
		/// Se usarán los niveles mínimo y máximo especificados
		/// en GpuImageSamplerDesc::minMipLevel y GpuImageSamplerDesc::maxMipLevel.
		/// </summary>
		CUSTOM

	};


	/// <summary>
	/// Establece propiedades básicas sobre el
	/// acceso a la imagen.
	/// </summary>
	struct GpuImageSamplerDesc {
		GpuImageFilteringType filteringType = GpuImageFilteringType::LIENAR;
		GpuImageAddressMode addressMode = GpuImageAddressMode::REPEAT;

		GpuImageMipmapMode mipMapMode = GpuImageMipmapMode::AUTO;


		/// <summary>
		/// Divel de mipmap más bajo.
		/// </summary>
		/// 
		/// @note El valor 0 identifica a la imagen original.
		/// @warning Únicamente se usa este valor si mipMapMode == GpuImageMipmapMode::CUSTOM.
		TSize minMipLevel = 0;

		/// <summary>
		/// Divel de mipmap más alto.
		/// </summary>
		/// 
		/// @note El valor 0 identifica a la imagen original.
		/// @warning Únicamente se usa este valor si mipMapMode == GpuImageMipmapMode::CUSTOM.
		TSize maxMipLevel = 0;

		static GpuImageSamplerDesc CreateDefault() {
			return GpuImageSamplerDesc{ .mipMapMode = GpuImageMipmapMode::NONE };
		}

		static GpuImageSamplerDesc CreateTextureDefault() {
			return GpuImageSamplerDesc{ .mipMapMode = GpuImageMipmapMode::AUTO };
		}

	};

}
