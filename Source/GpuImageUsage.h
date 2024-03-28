#pragma once

#include "EnumFlags.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Uso que se le va a dar a la imagen.
	/// 
	/// @note Se pueden combinar varios de estos usos.
	/// </summary>
	enum class GpuImageUsage {

		UNKNOWN = 0,

		/// <summary>
		/// Se usar� como imagen final para el renderizado 3D.
		/// Almacenar� la imagen de color final.
		/// </summary>
		COLOR = 1,

		/// <summary>
		/// Se usar� como imagen de profundidad para el renderizado 3D.
		/// </summary>
		DEPTH = 2,

		/// <summary>
		/// Se usar� como read-only desde los shaders.
		/// </summary>
		SAMPLED = 4,

		/// <summary>
		/// Se usar� para copiar esta imagen a otra.
		/// </summary>
		TRANSFER_SOURCE = 8,

		/// <summary>
		/// Se copiar�n los contenidos de otra imagen a esta.
		/// </summary>
		TRANSFER_DESTINATION = 16,

		/// <summary>
		/// Se usar� como un cubemap.
		/// </summary>
		CUBEMAP = 32,

		/// <summary>
		/// Se usar� como imagen sobre la que se renderizar� el
		/// trazado de rayos.
		/// </summary>
		RT_TARGET_IMAGE = 64,

		/// <summary>
		/// Se usar� como read-only desde los shaders.
		/// Es un array de im�genes.
		/// </summary>
		SAMPLED_ARRAY = 128,

		/// <summary>
		/// Usado en shaders de computaci�n.
		/// </summary>
		COMPUTE = 256,

		/// <summary>
		/// Se usar� como imagen de stencil para el renderizado 3D.
		/// </summary>
		STENCIL = 512,

	};


	/// <summary>
	/// Define qu� parte de una imagen se leer� al
	/// usar una imagen en un shader.
	/// </summary>
	enum class SampledChannel {

		/// <summary>
		/// La imagen es una imagen de color.
		/// Es el funcionamiento por defecto.
		/// </summary>
		COLOR = 1,

		/// <summary>
		/// Obtiene la parte de profundidad de una
		/// imagen de depth/stencil.
		/// 
		/// @pre La imagen debe haber sido creada con
		/// GpuImageUsage::DEPTH_STENCIL y GpuImageUsage::SAMPLED.
		/// </summary>
		DEPTH = 2,

		/// <summary>
		/// Obtiene la parte de stencil de una
		/// imagen de depth/stencil.
		/// 
		/// @pre La imagen debe haber sido creada con
		/// GpuImageUsage::DEPTH_STENCIL y GpuImageUsage::SAMPLED.
		/// </summary>
		STENCIL = 4

	};


	enum class SampledArrayType {
		SINGLE_LAYER,
		ARRAY
	};

	/// @brief Describe c�mo se almacena la imagen en la memoria.
	enum class GpuImageTiling {
		/// @brief La imagen se almacena de una forma �ptima para su uso en el motor.
		OPTIMAL,

		/// @brief La imagen se almacena de manera que pueda copiarse a la CPU de manera sencilla.
		LINEAL
	};

}

OSK_FLAGS(OSK::GRAPHICS::GpuImageUsage);
OSK_FLAGS(OSK::GRAPHICS::SampledChannel);
