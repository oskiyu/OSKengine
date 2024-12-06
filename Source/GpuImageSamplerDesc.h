#pragma once

#include "ApiCall.h"
#include "NumericTypes.h"

#include <type_traits>


namespace OSK::GRAPHICS {

	class GpuImage;

	/// @brief Tipo de filtro que se aplicará al acceder a la imagen.
	enum class GpuImageFilteringType {

		/// @brief Valor linealmente interpolado con los colindantes.
		LINEAR,

		/// @brief El valor no se interpola.
		NEAREST,

		/// @brief El valor se interpola cúbicamente con los colindantes.
		CUBIC

	};

	/// @brief Describe qué color se seleccionará al acceder fuera de las
	/// coordenadas de la textura.
	enum class GpuImageAddressMode {

		/// @brief La textura se repite infinitamente.
		REPEAT,

		/// @brief La textura se repite infinitamente, 
		/// dándose la vuelta en cada repetición.
		MIRRORED_REPEAT,
				
		/// @brief Se devuelve el color del borde más cercano.
		EDGE,

		/// @brief Color negro.
		BACKGROUND_BLACK,

		/// @brief Color blanco.
		BACKGROUND_WHITE,

	};


	/// @brief Configuración del nivel de mipmaps mínimo y máximo para imágenes
	/// en la GPU.
	enum class GpuImageMipmapMode {

		/// @brief No se usarán mipmaps.
		NONE,

		/// @brief Uso de mipmaps por defecto.
		/// 
		/// El nivel más bajo será 0 (imagen original),
		/// y el nivel más alto será el mayor posible, dependiendo
		/// del tamaño de la imagen.
		AUTO,

		/// @brief Se usarán los niveles mínimo y máximo especificados
		/// en GpuImageSamplerDesc::minMipLevel y GpuImageSamplerDesc::maxMipLevel.
		CUSTOM

	};


	/// @brief Establece propiedades básicas sobre el
	/// acceso a la imagen.
	struct OSKAPI_CALL GpuImageSamplerDesc {

		bool operator==(const GpuImageSamplerDesc& other) const = default;

		GpuImageFilteringType filteringType = GpuImageFilteringType::LINEAR;
		GpuImageAddressMode addressMode = GpuImageAddressMode::REPEAT;

		GpuImageMipmapMode mipMapMode = GpuImageMipmapMode::AUTO;

		/// @brief Nivel de mipmap más bajo.
		/// 
		/// @note El valor 0 identifica a la imagen original.
		/// @warning Únicamente se usa este valor si `mipMapMode == GpuImageMipmapMode::CUSTOM`.
		UIndex32 minMipLevel = 0;

		/// @brief Divel de mipmap más alto.
		/// 
		/// @note El valor 0 identifica a la imagen original.
		/// @warning Únicamente se usa este valor si mipMapMode == GpuImageMipmapMode::CUSTOM.
		UIndex32 maxMipLevel = 0;

		float mipBias = 0.0f;

		static GpuImageSamplerDesc CreateDefault_NoMipMap();
		static GpuImageSamplerDesc CreateDefault_WithMipMap(const GpuImage& image);

	};

}


template<> struct std::hash<OSK::GRAPHICS::GpuImageSamplerDesc> {
	std::size_t operator()(const OSK::GRAPHICS::GpuImageSamplerDesc& info) const {
		const size_t value =
			static_cast<size_t>(info.filteringType) +
			static_cast<size_t>(info.addressMode) * 10 +
			static_cast<size_t>(info.minMipLevel) * 100 +
			static_cast<size_t>(info.maxMipLevel) * 10000;

		return std::hash<std::size_t>()(value);
	}
};
