#pragma once

#include <compare>

#include "GpuImageUsage.h"
#include "OSKmacros.h"

namespace OSK::GRAPHICS {


	/// <summary>
	/// Representa el uso que se le va a dar a la imagen
	/// al acceder a traves de un view dado.
	/// </summary>
	enum class ViewUsage {

		/// <summary>
		/// Read-only en el fragment shader.
		/// </summary>
		SAMPLED,

		/// <summary>
		/// Se renderizará una imagen al canal de color
		/// usando un graphics pipeline.
		/// </summary>
		COLOR_TARGET,

		/// <summary>Se renderizará una imagen al canal de profundidad
		/// usando un graphics pipeline.
		/// </summary>
		DEPTH_STENCIL_TARGET,

		/// @brief Se renderizará una imagen al canal de profundidad
		/// usando un graphics pipeline.
		DEPTH_ONLY_TARGET,

		/// <summary>
		/// Uso como read-write en shaders de computación y
		/// ray-tracing.
		/// </summary>
		STORAGE

	};



	/// @brief Configuración de un image view.
	/// Desribe las características de un image view.
	struct GpuImageViewConfig {

		/// @brief Crea una configuración para un uso normal de una textura.
		/// Canal = SampledChannel::COLOR.
		/// Imagen simple que usa la primera imagen de array.
		/// Usa todos los mip-levels.
		static GpuImageViewConfig CreateSampled_Default();

		/// @brief Crea una configuración para un uso normal de una textura,
		/// en la que se usa únicamente el mip-level indicado.
		/// Canal = SampledChannel::COLOR.
		/// Imagen simple que usa la primera imagen de array.
		/// 
		/// @param mipLevel Mip level usado.
		/// 
		/// @pre @p mipLevel debe existir en la imagen original.
		static GpuImageViewConfig CreateSampled_SingleMipLevel(TIndex mipLevel);

		/// @brief Crea una configuración para un uso normal de una textura,
		/// en la que se usan los mip-levels del rango indicado.
		/// Canal = SampledChannel::COLOR.
		/// Imagen simple que usa la primera imagen de array.
		/// 
		/// @param baseMipLevel Nivel más detallado (por defecto 0 para el rango completo). Inclusive.
		/// @param topMipLevel Nivel menos detallado. Inclusive.
		/// 
		/// @pre Todos los niveles de mip indicados dentro del rango deben existir en la imagen original.
		/// @pre @p baseMipLevel <= @p topMipLevel.
		static GpuImageViewConfig CreateSampled_MipLevelRanged(TIndex baseMipLevel, TIndex topMipLevel);

		/// @brief Crea una configuración para un uso normal de una textura, que es un array.
		/// Usa todos los mip-levels.
		/// Canal = SampledChannel::COLOR.
		/// 
		/// @param baseLayer Primera capa del array. Por defecto, 0.
		/// @param layerCount Número de capas.
		/// 
		/// @pre @p layerCount >= 1.
		static GpuImageViewConfig CreateSampled_Array(TIndex baseLayer, TSize layerCount);

		/// @brief Crea una configuración para un uso normal de un cubemap.
		/// Usa todos los mip-levels.
		/// Canal = SampledChannel::COLOR.
		static GpuImageViewConfig CreateSampled_Cubemap();


		/// @brief Crea una configuración para un uso normal al escribir a una imagen de storage.
		/// Canal = SampledChannel::COLOR.
		/// Usa el mip-level 0.
		/// Usa el primer nivel de array.
		static GpuImageViewConfig CreateStorage_Default();


		/// @brief Crea una configuración para renderizar a una imagen de color.
		/// Usa el mip-level 0.
		/// Usa el primer nivel de array.
		static GpuImageViewConfig CreateTarget_Color();

		/// @brief Crea una configuración para renderizar a una imagen de profundidad.
		/// Usa el mip-level 0.
		/// Usa el primer nivel de array.
		static GpuImageViewConfig CreateTarget_Depth();

		/// @brief Crea una configuración para renderizar a una imagen de stencil.
		/// Usa el mip-level 0.
		/// Usa el primer nivel de array.
		static GpuImageViewConfig CreateTarget_Stencil();

		/// @brief Crea una configuración para renderizar a una imagen de profundidad Y stencil.
		/// Usa el mip-level 0.
		/// Usa el primer nivel de array.
		static GpuImageViewConfig CreateTarget_DepthStencil();


		auto operator<=>(const GpuImageViewConfig&) const = default;


		/// @brief Canal de la imagen que se va a acceder.
		/// Por defecto, COLOR.
		/// 
		/// @note Se pueden combinar varios de estos canales.
		SampledChannel channel = SampledChannel::COLOR;

		/// @brief Representa si es una única imagen (o capa)
		/// o si representa un conjunto de capsa (array).
		/// Por defecto, una única capa.
		/// 
		/// @note Si se va a representar un único cubemap, debe ser SampledArrayType::SINGLE_LAYER. 
		SampledArrayType arrayType = SampledArrayType::SINGLE_LAYER;

		/// @brief Capa base del array.
		/// Para imágenes simples, debe ser 0.
		/// 
		/// @pre Para cubemaps, debe ser múltiplo de 6.
		TIndex baseArrayLevel = 0;

		/// @brief Número de capas del array.
		/// 
		/// @pre Para imágenes simples, debe ser 1.
		/// @pre Para arrays, debe ser > 1.
		/// @pre Para un único cubemap, debe ser 6 y @p arrayType debe ser SampledArrayType::SINGLE_LAYER.
		/// @pre Para array de cubemaps, debe ser múltiplo de 6 y @p arrayType debe ser SampledArrayType::ARRAY.
		TSize arrayLevelCount = 1;

		/// @brief Detalle máximo de mip-level.
		/// Por defecto, 0 (más alto).
		/// 
		/// @pre Debe ser menor o igual que @p topMipLevel.
		/// @pre Para usar un único mip-level, debe ser igual a @p topMipLevel.
		TIndex baseMipLevel = 0;

		/// @brief Detalle mínimo del mip-level.
		/// 
		/// @pre Debe ser mayor o igual que @p baseMipLevel.
		/// @pre Para usar un único mip-level, debe ser igual a @p baseMipLevel.
		TIndex topMipLevel = 0;

		/// @brief Uso que se le va a dar.
		/// Por defecto, SAMPLED.
		ViewUsage usage = ViewUsage::SAMPLED;

		/// @brief Configuración sobre los mip-maps.
		enum class MipMapMode {
			/// @brief Se tendrá acceso a todos los mip-levels.
			/// Tanto @p baseMipLevel como @p topMipLevel serán ignorados.
			ALL_LEVELS,
			/// @brief Se usará el rango de niveles definidos por @p baseMipLevel y @p topMipLevel.
			LEVEL_RANGE
		}

		/// @brief Configuración sobre los mip-maps.
		/// Por defecto, usará todos los niveles.
		mipMapMode = MipMapMode::ALL_LEVELS;

	};

}

template<> struct std::hash<OSK::GRAPHICS::GpuImageViewConfig> {
	std::size_t operator()(const OSK::GRAPHICS::GpuImageViewConfig& viewConfig) const {
		return std::hash<std::size_t>()(
			(std::size_t)(viewConfig.usage));
		// @todo improve
	}
};
