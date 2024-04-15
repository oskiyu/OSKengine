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

	/// @brief Estructura que contiene la informaci�n necesaria
	/// para la creaci�n de una imagen.
	struct GpuImageCreateInfo {

		/// @brief Inicializa la estructura para im�genes de 1D.
		/// @param resolution Resoluci�n de la imagen, en n�mero de p�xeles.
		/// @param format Formato de la imagen.
		/// @param usage Uso de la imagen.
		/// @return Estructura con los campos correctamente inicializados para 
		/// im�genes 1D.
		/// 
		/// @note @p numLayers ser� 1.
		/// @note @p msaaSamples ser� 1.
		/// @note @p samplerDesc ser� el creado por defecto.
		/// @note @p memoryType ser� `GpuSharedMemoryType::GPU_ONLY`.
		/// @note @p tilingType ser� `GpuImageTiling::OPTIMAL`.
		/// @note @p dimension ser� `GpuImageDimension::d1D`.
		/// @note @p resourceUsage ser� `GpuResourceUsage::GRAPHICS_AND_COMPUTE`.
		/// @warning @p usage ser� `GpuImageUsage::UNKNOWN`.
		static GpuImageCreateInfo CreateDefault1D(
			USize32 resolution, 
			Format format, 
			GpuImageUsage usage);

		/// @brief Inicializa la estructura para im�genes de 2D.
		/// @param resolution Resoluci�n de la imagen, en n�mero de p�xeles.
		/// @param format Formato de la imagen.
		/// @param usage Uso de la imagen.
		/// @return Estructura con los campos correctamente inicializados para 
		/// im�genes 1D.
		/// 
		/// @note @p numLayers ser� 1.
		/// @note @p msaaSamples ser� 1.
		/// @note @p samplerDesc ser� el creado por defecto.
		/// @note @p memoryType ser� `GpuSharedMemoryType::GPU_ONLY`.
		/// @note @p tilingType ser� `GpuImageTiling::OPTIMAL`.
		/// @note @p dimension ser� `GpuImageDimension::d2D`.
		/// @note @p resourceUsage ser� `GpuResourceUsage::GRAPHICS_AND_COMPUTE`.
		/// @warning @p usage ser� `GpuImageUsage::UNKNOWN`.
		static GpuImageCreateInfo CreateDefault2D(
			const Vector2ui& resolution, 
			Format format, 
			GpuImageUsage usage);

		/// @brief Inicializa la estructura para im�genes de 3D.
		/// @param resolution Resoluci�n de la imagen, en n�mero de p�xeles.
		/// @param format Formato de la imagen.
		/// @param usage Uso de la imagen.
		/// @return Estructura con los campos correctamente inicializados para 
		/// im�genes 1D.
		/// 
		/// @note @p numLayers ser� 1.
		/// @note @p msaaSamples ser� 1.
		/// @note @p samplerDesc ser� el creado por defecto.
		/// @note @p memoryType ser� `GpuSharedMemoryType::GPU_ONLY`.
		/// @note @p tilingType ser� `GpuImageTiling::OPTIMAL`.
		/// @note @p dimension ser� `GpuImageDimension::d3D`.
		/// @note @p resourceUsage ser� `GpuResourceUsage::GRAPHICS_AND_COMPUTE`.
		/// @warning @p usage ser� `GpuImageUsage::UNKNOWN`.
		static GpuImageCreateInfo CreateDefault3D(
			const Vector3ui& resolution, 
			Format format, 
			GpuImageUsage usage);


		/// @brief Resoluci�n de la imagen, en p�xeles.
		/// @details Para im�genes 2D, Z se ignora. 
		/// @details Para im�genes 1D, Y y Z se ignoran.
		Vector3ui resolution;

		/// @brief Formato de la imagen.
		Format format;

		/// @brief Tareas para las que se usar� la imagen.
		GpuImageUsage usage = GpuImageUsage::UNKNOWN;

		/// @brief N�mero de dimensiones de la imagen.
		GpuImageDimension dimension;

		/// @brief N�mero de capas.
		/// Si se trata de una imagen sencilla, debe ser 1.
		/// Si se trata de un array de im�genes, debe ser >= 2.
		/// 
		/// @pre Debe ser > 0.
		USize32 numLayers = 1;

		/// @brief N�mero de muestreos.
		/// Para im�genes normales, 1.
		USize32 msaaSamples = 1;

		/// @brief Estructura con informaci�n sobre c�mo se acceder� a la imagen desde los shaders.
		GpuImageSamplerDesc samplerDesc = {};

		/// @brief Tipo de memoria en la que se alojar� la imagen.
		GpuSharedMemoryType memoryType = GpuSharedMemoryType::GPU_ONLY;

		/// @brief Estructura de almacenamiento de la imagen.
		GpuImageTiling tilingType = GpuImageTiling::OPTIMAL;

		/// @brief Tipo de cola sobre la que se alojar�.
		GpuQueueType queueType = GpuQueueType::MAIN;

	};

}
