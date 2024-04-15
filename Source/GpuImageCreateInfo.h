#pragma once

#include "OSKmacros.h"

#include "Vector2.hpp"
#include "Vector3.hpp"

#include "Format.h"
#include "GpuImageUsage.h"
#include "GpuImageDimensions.h"
#include "GpuImageSamplerDesc.h"
#include "GpuMemoryTypes.h"
#include "GpuQueueTypes.h"


namespace OSK::GRAPHICS {

	/// @brief Estructura que contiene la información necesaria
	/// para la creación de una imagen.
	struct GpuImageCreateInfo {

		/// @brief Inicializa la estructura para imágenes de 1D.
		/// @param resolution Resolución de la imagen, en número de píxeles.
		/// @param format Formato de la imagen.
		/// @param usage Uso de la imagen.
		/// @return Estructura con los campos correctamente inicializados para 
		/// imágenes 1D.
		/// 
		/// @note @p numLayers será 1.
		/// @note @p msaaSamples será 1.
		/// @note @p samplerDesc será el creado por defecto.
		/// @note @p memoryType será `GpuSharedMemoryType::GPU_ONLY`.
		/// @note @p tilingType será `GpuImageTiling::OPTIMAL`.
		/// @note @p dimension será `GpuImageDimension::d1D`.
		/// @note @p resourceUsage será `GpuResourceUsage::GRAPHICS_AND_COMPUTE`.
		/// @warning @p usage será `GpuImageUsage::UNKNOWN`.
		static GpuImageCreateInfo CreateDefault1D(
			USize32 resolution, 
			Format format, 
			GpuImageUsage usage);

		/// @brief Inicializa la estructura para imágenes de 2D.
		/// @param resolution Resolución de la imagen, en número de píxeles.
		/// @param format Formato de la imagen.
		/// @param usage Uso de la imagen.
		/// @return Estructura con los campos correctamente inicializados para 
		/// imágenes 1D.
		/// 
		/// @note @p numLayers será 1.
		/// @note @p msaaSamples será 1.
		/// @note @p samplerDesc será el creado por defecto.
		/// @note @p memoryType será `GpuSharedMemoryType::GPU_ONLY`.
		/// @note @p tilingType será `GpuImageTiling::OPTIMAL`.
		/// @note @p dimension será `GpuImageDimension::d2D`.
		/// @note @p resourceUsage será `GpuResourceUsage::GRAPHICS_AND_COMPUTE`.
		/// @warning @p usage será `GpuImageUsage::UNKNOWN`.
		static GpuImageCreateInfo CreateDefault2D(
			const Vector2ui& resolution, 
			Format format, 
			GpuImageUsage usage);

		/// @brief Inicializa la estructura para imágenes de 3D.
		/// @param resolution Resolución de la imagen, en número de píxeles.
		/// @param format Formato de la imagen.
		/// @param usage Uso de la imagen.
		/// @return Estructura con los campos correctamente inicializados para 
		/// imágenes 1D.
		/// 
		/// @note @p numLayers será 1.
		/// @note @p msaaSamples será 1.
		/// @note @p samplerDesc será el creado por defecto.
		/// @note @p memoryType será `GpuSharedMemoryType::GPU_ONLY`.
		/// @note @p tilingType será `GpuImageTiling::OPTIMAL`.
		/// @note @p dimension será `GpuImageDimension::d3D`.
		/// @note @p resourceUsage será `GpuResourceUsage::GRAPHICS_AND_COMPUTE`.
		/// @warning @p usage será `GpuImageUsage::UNKNOWN`.
		static GpuImageCreateInfo CreateDefault3D(
			const Vector3ui& resolution, 
			Format format, 
			GpuImageUsage usage);


		/// @brief Resolución de la imagen, en píxeles.
		/// @details Para imágenes 2D, Z se ignora. 
		/// @details Para imágenes 1D, Y y Z se ignoran.
		Vector3ui resolution;

		/// @brief Formato de la imagen.
		Format format;

		/// @brief Tareas para las que se usará la imagen.
		GpuImageUsage usage = GpuImageUsage::UNKNOWN;

		/// @brief Número de dimensiones de la imagen.
		GpuImageDimension dimension;

		/// @brief Número de capas.
		/// Si se trata de una imagen sencilla, debe ser 1.
		/// Si se trata de un array de imágenes, debe ser >= 2.
		/// 
		/// @pre Debe ser > 0.
		USize32 numLayers = 1;

		/// @brief Número de muestreos.
		/// Para imágenes normales, 1.
		USize32 msaaSamples = 1;

		/// @brief Estructura con información sobre cómo se accederá a la imagen desde los shaders.
		GpuImageSamplerDesc samplerDesc = {};

		/// @brief Tipo de memoria en la que se alojará la imagen.
		GpuSharedMemoryType memoryType = GpuSharedMemoryType::GPU_ONLY;

		/// @brief Estructura de almacenamiento de la imagen.
		GpuImageTiling tilingType = GpuImageTiling::OPTIMAL;

		/// @brief Tipo de cola sobre la que se alojará.
		GpuQueueType queueType = GpuQueueType::MAIN;

	};

}
