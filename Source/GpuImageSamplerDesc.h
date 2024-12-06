#pragma once

#include "ApiCall.h"
#include "NumericTypes.h"

#include <type_traits>


namespace OSK::GRAPHICS {

	class GpuImage;

	/// @brief Tipo de filtro que se aplicar� al acceder a la imagen.
	enum class GpuImageFilteringType {

		/// @brief Valor linealmente interpolado con los colindantes.
		LINEAR,

		/// @brief El valor no se interpola.
		NEAREST,

		/// @brief El valor se interpola c�bicamente con los colindantes.
		CUBIC

	};

	/// @brief Describe qu� color se seleccionar� al acceder fuera de las
	/// coordenadas de la textura.
	enum class GpuImageAddressMode {

		/// @brief La textura se repite infinitamente.
		REPEAT,

		/// @brief La textura se repite infinitamente, 
		/// d�ndose la vuelta en cada repetici�n.
		MIRRORED_REPEAT,
				
		/// @brief Se devuelve el color del borde m�s cercano.
		EDGE,

		/// @brief Color negro.
		BACKGROUND_BLACK,

		/// @brief Color blanco.
		BACKGROUND_WHITE,

	};


	/// @brief Configuraci�n del nivel de mipmaps m�nimo y m�ximo para im�genes
	/// en la GPU.
	enum class GpuImageMipmapMode {

		/// @brief No se usar�n mipmaps.
		NONE,

		/// @brief Uso de mipmaps por defecto.
		/// 
		/// El nivel m�s bajo ser� 0 (imagen original),
		/// y el nivel m�s alto ser� el mayor posible, dependiendo
		/// del tama�o de la imagen.
		AUTO,

		/// @brief Se usar�n los niveles m�nimo y m�ximo especificados
		/// en GpuImageSamplerDesc::minMipLevel y GpuImageSamplerDesc::maxMipLevel.
		CUSTOM

	};


	/// @brief Establece propiedades b�sicas sobre el
	/// acceso a la imagen.
	struct OSKAPI_CALL GpuImageSamplerDesc {

		bool operator==(const GpuImageSamplerDesc& other) const = default;

		GpuImageFilteringType filteringType = GpuImageFilteringType::LINEAR;
		GpuImageAddressMode addressMode = GpuImageAddressMode::REPEAT;

		GpuImageMipmapMode mipMapMode = GpuImageMipmapMode::AUTO;

		/// @brief Nivel de mipmap m�s bajo.
		/// 
		/// @note El valor 0 identifica a la imagen original.
		/// @warning �nicamente se usa este valor si `mipMapMode == GpuImageMipmapMode::CUSTOM`.
		UIndex32 minMipLevel = 0;

		/// @brief Divel de mipmap m�s alto.
		/// 
		/// @note El valor 0 identifica a la imagen original.
		/// @warning �nicamente se usa este valor si mipMapMode == GpuImageMipmapMode::CUSTOM.
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
